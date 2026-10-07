# LinkCard e-Reader Reverse Transfer — investigación inconclusa

> **Estado:** investigación / prueba de concepto. **No es un distribuidor de tarjetas terminado ni estable.**

Este repositorio conserva el mínimo práctico de una investigación para enviar datos de una tarjeta Nintendo e-Reader desde una Game Boy Advance a otra GBA con un **e-Reader japonés**, usando un cable Link y un pequeño loader ejecutado en el e-Reader.

El trabajo parte de **gba-link-connection / LinkCard** y conserva su licencia original. Las modificaciones de este repositorio son experimentales y se documentan aquí para que los resultados no se pierdan.

## Objetivo

La idea original era convertir una GBA con flashcart en una biblioteca de tarjetas: elegir un archivo `.bin` de 2112 bytes y transmitirlo al e-Reader, evitando tener que escanear físicamente el dot code.

El proyecto consiguió demostrar la **transferencia inversa de los 2112 bytes** y transmitir una tarjeta japonesa real. Sin embargo, no se consiguió reproducir de forma estable todo el procesamiento que realiza el firmware después de un escaneo óptico. Tras varias transferencias consecutivas el loader/e-Reader puede quedar temporalmente ocupado, mostrar artefactos gráficos o cancelar la siguiente sesión. Por eso el proyecto se archiva como investigación inconclusa.

## Qué se consiguió

- Comunicación física GBA ↔ e-Reader por cable Link.
- Envío del loader al e-Reader japonés mediante el mecanismo utilizado por LinkCard.
- Recepción original e-Reader → GBA funcionando.
- Protocolo experimental inverso GBA → e-Reader.
- Transferencia completa de **2112 bytes** con checksum y ACK.
- Prueba con un `.bin` japonés real de *Doubutsu no Mori Card-e*.
- Conversión experimental del layout estándar de tarjeta: contenido de 1998 bytes desde `0x72` hacia el buffer utilizado desde `0x4E`.
- Llamada experimental a la función ERAPI no documentada `0x2C3` después de recibir la tarjeta.
- Varias transferencias consecutivas pueden funcionar antes de aparecer el fallo acumulativo.

## Qué NO está resuelto

El hecho de que aparezca `SUCCESS` solo confirma que nuestro protocolo terminó la transferencia; **no demuestra que el firmware haya procesado/ejecutado correctamente la tarjeta virtual**.

Después de varias sesiones se observaron estados como:

```text
Reverse ERROR 3
Stage: 5
Expected: 63736  (0xF8F8)
Received: 63479  (0xF7F7 / CANCEL)
```

y, en otras ocasiones, `0xFFFF`. El número de transferencia en que sucede no es fijo. También se observó que, después de un envío aparentemente correcto, el e-Reader puede permanecer un tiempo mostrando las líneas de espera; durante ese período la otra GBA puede mostrar `Device ??` y posteriormente volver a detectarlo.

Esperar una cantidad fija de segundos no solucionó el problema. Esto apunta a un estado interno del loader/firmware que no estamos rearmando correctamente entre sesiones.

## Hallazgos técnicos

Una tarjeta `.bin` estándar usada en las pruebas mide **2112 bytes**. En el código original de LinkCard se manejan 1998 bytes útiles y un offset `0x4E`; para las pruebas inversas se transmitieron los 2112 bytes completos y después se copió el bloque de 1998 bytes desde `0x72` a `0x4E`.

El protocolo inverso experimental usa, entre otros, estos valores:

```text
GAME_SEND_CARD      = 0xEAEA
EREADER_RECV_READY  = 0xF8F8
GAME_SEND_START     = 0xFAFA
GAME_SEND_END       = 0xFCFA
EREADER_RECV_OK     = 0xF5FA
EREADER_RECV_ERROR  = 0xF4FA
EREADER_CANCEL      = 0xF7F7
```

Se probó `ERAPI 0x2C3`. Los valores mostrados después de la llamada no fueron constantes (`5172`, `4988`, `4980`, etc.), por lo que **no deben interpretarse como un código de éxito conocido**. La semántica de esta función sigue sin estar documentada.

También se llegó a probar `ERAPI 0x2DD` durante la investigación. Esa prueba produjo corrupción gráfica/congelamiento, por lo que **no se recomienda repetir llamadas a funciones ERAPI desconocidas a ciegas**. Ese experimento no está incluido en este snapshot mínimo.

## Hardware usado

- 2 × Game Boy Advance (o compatibles para el experimento).
- Cable GBA Link.
- Flashcart en la GBA distribuidora.
- Nintendo e-Reader japonés / e-Reader+ en la segunda GBA.

En las pruebas inversas, la GBA distribuidora actúa como el lado que proporciona el reloj de la comunicación y el loader se ejecuta en la GBA con e-Reader.

## Compilación

Se utilizó **devkitPro/devkitARM** y las herramientas de e-Reader (`nevpk`, `neflmake`, `nedcmake`, `nedcenc`, `raw2bmp`) instaladas en:

```text
/opt/devkitpro/_e-reader
```

Desde MSYS2/devkitPro:

```bash
cd examples/LinkCard_demo
./BUILD_REVERSE_TEST.sh
```

El script primero recompila el loader japonés en inglés, lo coloca en el GBFS y después genera la ROM empaquetada.

El archivo que debe cargarse en la flashcart es:

```text
LinkCard_demo.out.gba
```

**No** `LinkCard_demo.gba`, porque `.out.gba` contiene el GBFS con los loaders.

### Nota sobre rutas

El `Makefile` incluido fue adaptado para tolerar rutas del repositorio con espacios y paréntesis. `BUILD_REVERSE_TEST.sh` usa por defecto:

```bash
DEVKITPRO=/opt/devkitpro
DEVKITARM=/opt/devkitpro/devkitARM
```

Estas variables pueden definirse externamente si la instalación está en otro lugar. El Makefile del loader todavía espera las herramientas de e-Reader en `/opt/devkitpro/_e-reader`.

## Estructura mínima

```text
lib/
  LinkCard.hpp
  LinkRawCable.hpp
  LinkSPI.hpp
  _link_common.hpp

examples/
  gbfs.sh
  _lib/                 dependencias mínimas del demo
  LinkCard_demo/
    Makefile
    BUILD_REVERSE_TEST.sh
    src/
      main.cpp
      test_card.h
    gbfs_files/         loaders usados por la ROM
    #loader/            código que se ejecuta en el e-Reader
```

`test_card.h` contiene la tarjeta japonesa utilizada como payload de prueba, convertida a un array de 2112 bytes. No se incluye una colección de tarjetas ni una interfaz de biblioteca porque esa fase nunca se consideró estable.

## Si alguien quiere continuar la investigación

La prueba más útil pendiente probablemente sea separar dos problemas: **transporte** y **procesamiento ERAPI**. Una variante debería recibir muchas tarjetas consecutivas sin llamar a `0x2C3`; si eso permanece estable, el fallo acumulativo estaría relacionado con el procesamiento/rearmado posterior y no con el transporte de 2112 bytes. También sería útil entender de forma documentada qué hace realmente `0x2C3` antes de continuar probando funciones internas.

No recomiendo añadir reintentos ciegos por palabra: si ambos lados discrepan sobre si una transferencia terminó, repetir una palabra puede desalinear el stream.

## Estado final

Este código se publica principalmente para **documentar lo aprendido y servir como punto de partida para futuras investigaciones**. No hay garantía de estabilidad y no considero terminada la función de “tarjeta virtual”.

Si solo se desea estudiar el resultado alcanzado, esta versión es suficiente; las numerosas revisiones intermedias de diagnóstico fueron eliminadas deliberadamente para mantener el repositorio pequeño.

## Créditos y licencia

Este trabajo deriva de **gba-link-connection**, especialmente de su implementación `LinkCard`, y conserva el archivo `LICENSE` original. Los créditos y avisos de los proyectos de terceros deben mantenerse al redistribuir sus componentes.

Las modificaciones de transferencia inversa y las pruebas con el loader son trabajo experimental añadido sobre esa base; no implican autoría del código original ni documentación oficial de Nintendo.
