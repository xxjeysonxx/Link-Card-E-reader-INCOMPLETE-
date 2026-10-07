#include "def.h"
#include "erapi.h"
#include "link.h"
#include "protocol.h"

// Enable this to simulate failed scans with ⮜ and successful scans with ➤
#define DEBUG_MODE 0

// Enable this to display error codes
#define DISPLAY_ERROR_CODES 1

// Japanese strings are encoded as Shift-JIS byte arrays.
#ifdef REGION_JAP
#if DISPLAY_ERROR_CODES == 1
/* "０１２３４５６７８９" */
const u8 MSG_NUMBERS[] = {0x82, 0x4f, 0x82, 0x50, 0x82, 0x51, 0x82,
                          0x52, 0x82, 0x53, 0x82, 0x54, 0x82, 0x55,
                          0x82, 0x56, 0x82, 0x57, 0x82, 0x58, 0x00};
#endif

#ifdef LANGUAGE_ENG
const u8 MSG_WAITING_GAME[] = {
    0x82, 0x76, 0x82, 0x60, 0x82, 0x68, 0x82, 0x73, 0x82, 0x68, 0x82,
    0x6d, 0x82, 0x66, 0x81, 0x40, 0x82, 0x65, 0x82, 0x6e, 0x82, 0x71,
    0x81, 0x40, 0x82, 0x66, 0x82, 0x60, 0x82, 0x6c, 0x82, 0x64, 0x00};
const u8 MSG_SCAN_CARD[] = {
    0x82, 0x6f, 0x82, 0x6b, 0x82, 0x64, 0x82, 0x60, 0x82, 0x72, 0x82,
    0x64, 0x81, 0x40, 0x82, 0x72, 0x82, 0x62, 0x82, 0x60, 0x82, 0x6d,
    0x81, 0x40, 0x82, 0x78, 0x82, 0x6e, 0x82, 0x74, 0x82, 0x71, 0x81,
    0x40, 0x82, 0x62, 0x82, 0x60, 0x82, 0x71, 0x82, 0x63, 0x00};
const u8 MSG_TRANSFERRING[] = {0x82, 0x73, 0x82, 0x71, 0x82, 0x60, 0x82,
                               0x6d, 0x82, 0x72, 0x82, 0x65, 0x82, 0x64,
                               0x82, 0x71, 0x82, 0x71, 0x82, 0x68, 0x82,
                               0x6d, 0x82, 0x66, 0x00};
const u8 MSG_CARD_SENT[] = {0x82, 0x62, 0x82, 0x60, 0x82, 0x71, 0x82,
                            0x63, 0x81, 0x40, 0x82, 0x72, 0x82, 0x64,
                            0x82, 0x6d, 0x82, 0x73, 0x00};
const u8 MSG_ERROR[] = {0x82, 0x64, 0x82, 0x71, 0x82, 0x71,
                        0x82, 0x6e, 0x82, 0x71, 0x00};
const u8 MSG_PRESS_B_CANCEL[] = {
    0x82, 0x6f, 0x82, 0x71, 0x82, 0x64, 0x82, 0x72, 0x82, 0x72, 0x81, 0x40,
    0x82, 0x61, 0x81, 0x40, 0x82, 0x73, 0x82, 0x6e, 0x81, 0x40, 0x82, 0x62,
    0x82, 0x60, 0x82, 0x6d, 0x82, 0x62, 0x82, 0x64, 0x82, 0x6b, 0x00};
#else
const u8 MSG_WAITING_GAME[] = {0x83, 0x51, 0x81, 0x5b, 0x83, 0x80, 0x82,
                               0xf0, 0x91, 0xd2, 0x82, 0xc1, 0x82, 0xc4,
                               0x82, 0xa2, 0x82, 0xdc, 0x82, 0xb7, 0x00};
const u8 MSG_SCAN_CARD[] = {0x83, 0x4a, 0x81, 0x5b, 0x83, 0x68, 0x82, 0xf0,
                            0x83, 0x58, 0x83, 0x4c, 0x83, 0x83, 0x83, 0x93,
                            0x82, 0xb5, 0x82, 0xc4, 0x82, 0xad, 0x82, 0xbe,
                            0x82, 0xb3, 0x82, 0xa2, 0x00};
const u8 MSG_TRANSFERRING[] = {0x93, 0x5d, 0x91, 0x97, 0x92, 0x86, 0x00};
const u8 MSG_CARD_SENT[] = {0x83, 0x4a, 0x81, 0x5b, 0x83, 0x68, 0x91, 0x97,
                            0x90, 0x4d, 0x8d, 0xcf, 0x82, 0xdd, 0x00};
const u8 MSG_ERROR[] = {0x83, 0x47, 0x83, 0x89, 0x81, 0x5b, 0x00};
const u8 MSG_PRESS_B_CANCEL[] = {0x83, 0x72, 0x81, 0x5b, 0x82, 0xf0, 0x89, 0x9f,
                                 0x82, 0xb5, 0x82, 0xc4, 0x83, 0x4c, 0x83, 0x83,
                                 0x83, 0x93, 0x83, 0x5a, 0x83, 0x8b, 0x00};
#endif
#else
const char* MSG_WAITING_GAME = "Waiting for game...";
const char* MSG_SCAN_CARD = "Scan a card!";
const char* MSG_TRANSFERRING = "Transferring...";
const char* MSG_CARD_SENT = "Card sent!";
const char* MSG_ERROR = "Error!";
const char* MSG_PRESS_B_CANCEL = "Press B to cancel";
#endif

#define CARD_BUFFER_SIZE 2112  // Must hold a complete standard .bin reverse-transfer payload
#define SCAN_SUCCESS 6
#define POST_TRANSFER_WAIT 60
#define SESSION_DRAIN_FRAMES 8
#define SESSION_IDLE_FRAMES 12
#define STANDARD_BIN_HEADER_SIZE 0x72
#define SCAN_BUFFER_DATA_OFFSET  0x4e
#define CARD_CONTENT_SIZE        1998

extern int __end[];

ERAPI_HANDLE_REGION region;
u8 card[CARD_BUFFER_SIZE];
const u16 palette[] = {0x0000, 0xFFFF};
u32 previousKeys = 0;

// Reverse-transfer diagnostics shown on the e-Reader when validation fails.
u32 reverseDiagStage = 0;
u32 reverseDiagWord = 0;
u32 reverseDiagExpected = 0;
u32 reverseDiagReceived = 0;

#if DISPLAY_ERROR_CODES == 1
void codeToString(char* buf, int num) {
#ifdef REGION_JAP
  const u8* digits = MSG_NUMBERS;
  int temp[5];
  int len = 0;
  do {
    temp[len++] = num % 10;
    num /= 10;
  } while (num && len < 5);

  u8* p = (u8*)buf;
  for (int i = len - 1; i >= 0; --i) {
    int idx = temp[i] * 2;
    *p++ = digits[idx];
    *p++ = digits[idx + 1];
  }
  *p = 0;
#else
  char temp[6];
  int pos = 0;
  do {
    temp[pos++] = '0' + (num % 10);
    num /= 10;
  } while (num && pos < 5);
  int j = 0;
  while (pos)
    buf[j++] = temp[--pos];
  buf[j] = '\0';
#endif
}
#endif

void print(const char* text, bool canCancel);
bool cancel();
void reset();

bool receiveReverseCard() {
  u16 incoming = 0;
  reverseDiagStage = 5;
  reverseDiagWord = 0;
  reverseDiagExpected = EREADER_RECV_READY;
  reverseDiagReceived = 0xffff;

  u32 readyPolls = 0;
  do {
    if (!exchangeWithPlayer0(EREADER_RECV_READY, &incoming, cancel)) {
      reverseDiagReceived = 0xfffe;
      return false;
    }
    reverseDiagReceived = incoming;
    reverseDiagWord = readyPolls;
    readyPolls++;
  } while (incoming != GAME_SEND_START && readyPolls < 256);

  if (incoming != GAME_SEND_START) {
    reverseDiagExpected = GAME_SEND_START;
    return false;
  }

  reverseDiagStage = 6;
  u32 checksum = 0;
  for (u32 i = 0; i < REVERSE_CARD_SIZE; i += 2) {
    reverseDiagWord = i >> 1;
    if (!exchangeWithPlayer0(EREADER_RECV_DATA, &incoming, cancel)) {
      reverseDiagReceived = 0xfffe;
      return false;
    }
    card[i] = incoming & 0xff;
    card[i + 1] = incoming >> 8;
    checksum += incoming;
  }

  reverseDiagStage = 7;
  if (!exchangeWithPlayer0(EREADER_RECV_DATA, &incoming, cancel)) return false;
  u16 expectedLow = incoming;
  reverseDiagReceived = incoming;

  reverseDiagStage = 8;
  if (!exchangeWithPlayer0(EREADER_RECV_DATA, &incoming, cancel)) return false;
  u16 expectedHigh = incoming;
  reverseDiagReceived = incoming;

  reverseDiagStage = 9;
  reverseDiagExpected = GAME_SEND_END;
  if (!exchangeWithPlayer0(EREADER_RECV_DATA, &incoming, cancel)) return false;
  reverseDiagReceived = incoming;
  if (incoming != GAME_SEND_END) return false;

  u32 expectedChecksum = ((u32)expectedHigh << 16) | expectedLow;
  if (checksum != expectedChecksum) {
    reverseDiagStage = 11;
    reverseDiagExpected = expectedChecksum;
    reverseDiagReceived = checksum;
    return false;
  }

  // Real .bin mode: accept arbitrary card bytes after checksum validation.


  reverseDiagStage = 10;
  reverseDiagExpected = GAME_RECV_ACK;
  u32 okPolls = 0;
  do {
    reverseDiagWord = okPolls;
    if (!exchangeWithPlayer0(EREADER_RECV_OK, &incoming, cancel)) {
      reverseDiagReceived = 0xfffe;
      return false;
    }
    reverseDiagReceived = incoming;
    okPolls++;
  } while (incoming != GAME_RECV_ACK && okPolls < 256);

  return incoming == GAME_RECV_ACK;
}

int main() {
  // init
  ERAPI_FadeIn(1);
  ERAPI_InitMemory((ERAPI_RAM_END - (u32)__end) >> 10);
  ERAPI_SetBackgroundMode(0);

  // palette
  ERAPI_SetBackgroundPalette(&palette[0], 0x00, 0x02);

  // region & text
  region = ERAPI_CreateRegion(0, 0, 1, 1, 28, 10);
  ERAPI_SetTextColor(region, 0x01, 0x00);

  // background
  ERAPI_LoadBackgroundSystem(3, 20);

  // loop
  while (1) {
    u32 errorCode = 0;

    // init loader
    reset();

    // "Waiting for game..."
    print(MSG_WAITING_GAME, false);

    // handshake with game
    if (!sendAndExpect(HANDSHAKE_1, HANDSHAKE_1, cancel))
      continue;
    if (!sendAndExpect(HANDSHAKE_2, HANDSHAKE_2, cancel))
      continue;
    if (!sendAndExpect(HANDSHAKE_3, HANDSHAKE_3, cancel))
      continue;

    // wait for card request
    u16 cardRequest = sendAndReceiveExcept(HANDSHAKE_3, HANDSHAKE_3, cancel);

    // Experimental reverse transfer: Game -> e-Reader.
    if (cardRequest == GAME_SEND_CARD) {
      print(MSG_TRANSFERRING, true);

      if (!receiveReverseCard()) {
        errorCode = 10;
        goto error;
      }

      // v10: normalize a standard 2112-byte .bin into the layout used by
      // this loader after ERAPI_ScanDotCode().  A normal .bin has a 0x72-byte
      // header followed by the 1998-byte card payload.  The LinkCard loader's
      // scan path consumes those 1998 bytes from offset 0x4e.
      //
      // Destination is below source, so a forward copy is overlap-safe.
      // Use volatile pointers so GCC 16 cannot fold this overlap-safe loop
      // into a libc memmove() call. This loader is linked without libc.
      volatile u8* dst = (volatile u8*)(card + SCAN_BUFFER_DATA_OFFSET);
      volatile const u8* src =
          (volatile const u8*)(card + STANDARD_BIN_HEADER_SIZE);
      for (u32 i = 0; i < CARD_CONTENT_SIZE; i++)
        dst[i] = src[i];

      // v11 experiment: 0x2C3 is the ERAPI entry immediately following
      // ScanDotCode (0x2C2), but its semantics are undocumented in the public
      // headers.  Call it only after a validated Japanese .bin has been
      // normalized to the same payload layout used by the optical scan path.
      // We do NOT jump into card data.  The return value is shown on-screen so
      // hardware testing can tell us whether 0x2C3 accepts/rejects the buffer.
      ERAPI_ClearRegion(region);
      ERAPI_DrawText(region, 0, 0, MSG_CARD_SENT);
      char diagStr[11];
      codeToString(diagStr, 11);
      ERAPI_DrawText(region, 0, 16, diagStr); // marker: about to call 02C3
      ERAPI_RenderFrame(1);

      u32 processResult = ERAPI_02C3((u32)card);

      ERAPI_ClearRegion(region);
      ERAPI_DrawText(region, 0, 0, MSG_CARD_SENT);
      codeToString(diagStr, 12);
      ERAPI_DrawText(region, 0, 16, diagStr); // marker: 02C3 returned
      codeToString(diagStr, processResult & 0xffff);
      ERAPI_DrawText(region, 0, 32, diagStr);
      ERAPI_RenderFrame(1);
      for (u32 i = 0; i < POST_TRANSFER_WAIT * 3; i++)
        ERAPI_RenderFrame(1);

      // v13: keep the DLC loader resident after a successful reverse transfer.
      // Returning ERAPI_EXIT_TO_MENU unloads the loader, so a second transfer
      // sees stale/cancel protocol state (F7F7) until the loader is injected again.
      //
      // The outer loop calls reset() before the next handshake, restoring
      // multiplayer SIO mode and clearing the card buffer. This lets the same
      // loader accept another .bin without rebooting/reinjecting the e-Reader.
      // v14: explicitly tear down the completed multiplayer session before
      // advertising a new one.  Player 0 can still be clocking/polling the
      // final ACK when we get here; immediately resetting SIO made the next
      // session consume stale words and eventually answer F7F7/CANCEL.
      //
      // 1) leave the final ACK visible briefly while Player 0 exits its poll;
      // 2) disable multiplayer SIO completely;
      // 3) wait in neutral GPIO mode;
      // 4) the outer loop's reset() starts a fresh multiplayer session.
      for (u32 i = 0; i < SESSION_DRAIN_FRAMES; i++)
        ERAPI_RenderFrame(1);

      stopTransfer();
      REG_SIOMLT_SEND = 0;
      setGeneralPurposeMode();

      for (u32 i = 0; i < SESSION_IDLE_FRAMES; i++)
        ERAPI_RenderFrame(1);

      previousKeys = 0;
      continue;
    }

    if (cardRequest != GAME_REQUEST) {
      errorCode = 1;
      goto error;
    }

    // confirm card request
    if (!sendAndExpect(GAME_ANIMATING, EREADER_ANIMATING, cancel))
      continue;
    if (!send(EREADER_ANIMATING, cancel))
      continue;

    // scan card
    if (!sendAndExpect(EREADER_READY, GAME_READY, cancel)) {
      errorCode = 2;
      goto error;
    }

    // "Scan a card!"
    print(MSG_SCAN_CARD, false);

#if DEBUG_MODE == 1
    u32 resultCode = 0;
    while (true) {
      u32 debugKeys = ERAPI_GetKeyStateRaw();
      if ((debugKeys & ERAPI_KEY_LEFT) != 0) {
        resultCode = SCAN_SUCCESS - 1;
        break;
      }
      if ((debugKeys & ERAPI_KEY_RIGHT) != 0) {
        resultCode = SCAN_SUCCESS;
        const char msg[] = "HelloWorld";
        const u32 msgLen = sizeof(msg) - 1;
        const u32 byteCount = CARD_BUFFER_SIZE - CARD_OFFSET;
        for (u32 i = 0; i < byteCount; i++)
          card[CARD_OFFSET + i] = i == byteCount - 1 ? '!' : msg[i % msgLen];
        break;
      }
    }
#else
    u32 resultCode = ERAPI_ScanDotCode((u32)card);
#endif

    if (resultCode != SCAN_SUCCESS) {
      errorCode = 3;
      goto error;
    }

    // v12 control experiment: call the same undocumented ERAPI 0x2C3 on
    // the buffer produced by a REAL optical scan.  v11 returned 4988
    // (0x137C) when 0x2C3 was called on our Link-Cable-injected/normalized
    // buffer.  Comparing the two return values tells us whether 0x2C3 sees
    // a meaningful difference between the real scanner output and our
    // reconstructed buffer.  This does not assume that 0x2C3 executes cards.
    u32 opticalProcessResult = ERAPI_02C3((u32)card);
    ERAPI_ClearRegion(region);
    ERAPI_DrawText(region, 0, 0, MSG_CARD_SENT);
    char opticalDiagStr[11];
    codeToString(opticalDiagStr, 21);
    ERAPI_DrawText(region, 0, 16, opticalDiagStr); // 21 = real optical path
    codeToString(opticalDiagStr, opticalProcessResult & 0xffff);
    ERAPI_DrawText(region, 0, 32, opticalDiagStr);
    ERAPI_RenderFrame(1);
    for (u32 i = 0; i < POST_TRANSFER_WAIT * 3; i++)
      ERAPI_RenderFrame(1);

    // Continue the original optical-card transfer after the diagnostic.
    print(MSG_TRANSFERRING, true);

    // transfer start
    if (!sendAndExpect(EREADER_SEND_READY, GAME_RECEIVE_READY, cancel)) {
      errorCode = 4;
      goto error;
    }
    if (!send(EREADER_SEND_START, cancel)) {
      errorCode = 5;
      goto error;
    }

    // transfer
    u32 checksum = 0;
    for (u32 o = CARD_OFFSET; o < CARD_SIZE; o += 2) {
      u16 block = *(u16*)(card + o);
      if (!send(block, cancel)) {
        errorCode = 6;
        goto error;
      }
      checksum += block;
    }
    if (!send(checksum & 0xffff, cancel)) {
      errorCode = 7;
      goto error;
    }
    if (!send(checksum >> 16, cancel)) {
      errorCode = 8;
      goto error;
    }
    if (!send(EREADER_SEND_END, cancel)) {
      errorCode = 9;
      goto error;
    }

    // "Card sent!"
    print(MSG_CARD_SENT, false);
    for (u32 i = 0; i < POST_TRANSFER_WAIT; i++)
      ERAPI_RenderFrame(1);

    continue;

  error:
    // "Error!"
    ERAPI_ClearRegion(region);
    ERAPI_DrawText(region, 0, 0, MSG_ERROR);
#if DISPLAY_ERROR_CODES == 1
    char errorCodeStr[11];
    codeToString(errorCodeStr, errorCode);
    ERAPI_DrawText(region, 0, 16, errorCodeStr);
    if (errorCode == 10) {
      char diagStr[11];
      codeToString(diagStr, reverseDiagStage);
      ERAPI_DrawText(region, 0, 32, diagStr);
      codeToString(diagStr, reverseDiagWord);
      ERAPI_DrawText(region, 0, 48, diagStr);
      codeToString(diagStr, reverseDiagExpected & 0xffff);
      ERAPI_DrawText(region, 0, 64, diagStr);
      codeToString(diagStr, reverseDiagReceived & 0xffff);
      ERAPI_DrawText(region, 0, 80, diagStr);
    }
#else
    ERAPI_DrawText(region, 0, 16, MSG_WAITING_GAME);
#endif
    ERAPI_RenderFrame(1);

    send(EREADER_CANCEL, cancel);
    send(EREADER_CANCEL, cancel);
    send(EREADER_ANIMATING, cancel);
    send(EREADER_SIO_END, cancel);
  }

  setGeneralPurposeMode();

  // exit
  return ERAPI_EXIT_TO_MENU;
}

void print(const char* text, bool canCancel) {
  ERAPI_ClearRegion(region);
  ERAPI_DrawText(region, 0, 0, text);
  if (canCancel)
    ERAPI_DrawText(region, 0, 16, MSG_PRESS_B_CANCEL);
  ERAPI_RenderFrame(1);
}

bool cancel() {
  u32 keys = ERAPI_GetKeyStateRaw();
  bool isPressed =
      (previousKeys & ERAPI_KEY_B) == 0 && (keys & ERAPI_KEY_B) != 0;
  previousKeys = keys;
  return isPressed;
}

void reset() {
  // v14 session re-arm: always begin from a neutral SIO state.
  stopTransfer();
  REG_SIOMLT_SEND = 0;
  setGeneralPurposeMode();
  previousKeys = 0;

  for (u32 i = 0; i < CARD_BUFFER_SIZE; i++)
    ((vu8*)card)[i] = 0;

  // Do not expose the next FBFB handshake until the previous master-side
  // transaction has had time to disappear from the cable.
  for (u32 i = 0; i < SESSION_IDLE_FRAMES; i++)
    ERAPI_RenderFrame(1);

  setMultiPlayMode(3);  // 3 = 115200 bps
}
