#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "def.h"

// This protocol is only meant to be used by homebrew games using
// gba-link-connection's LinkCard library and its loader. It's highly inspired
// by `4-e`, but not compatible with it since SMA4 cards start from offset
// `0x72`, while cards generated with `nedcmake v1.4` start from offset `0x4e`.
// Both card formats are valid, though. That said, you could make it compatible
// by changing `CARD_OFFSET` and `CARD_SIZE` to the commented values, or by just
// recompiling `4-e` to skip 0x4e bytes (no pun intended) instead of 0x72.

#define CARD_OFFSET /*0x72*/ 0x4e
#define CARD_SIZE /*2112*/ 2076

#define HANDSHAKE_1 0xfbfb
#define HANDSHAKE_2 0x5841
#define HANDSHAKE_3 0x4534

#define GAME_REQUEST 0xecec

#define GAME_ANIMATING 0xf3f3
#define EREADER_ANIMATING 0xf2f2
#define EREADER_READY 0xf1f1

#define GAME_READY 0xefef

#define EREADER_CANCEL 0xf7f7

#define EREADER_SEND_READY 0xf9f9
#define GAME_RECEIVE_READY 0xfefe

#define EREADER_SEND_START 0xfdfd
#define EREADER_SEND_END 0xfcfc

#define GAME_RECEIVE_OK 0xf5f5
#define GAME_RECEIVE_ERROR 0xf4f4

#define EREADER_SIO_END 0xf3f3
#define GAME_SIO_END 0xf1f1

// Experimental reverse transfer: Game -> e-Reader.
#define REVERSE_CARD_SIZE 2112
#define GAME_SEND_CARD 0xeaea
#define EREADER_RECV_READY 0xf8f8
#define GAME_SEND_START 0xfafa
#define EREADER_RECV_DATA 0xfbfa
#define GAME_SEND_END 0xfcfa
#define EREADER_RECV_OK 0xf5fa
#define GAME_RECV_ACK 0xf6fa

#endif
