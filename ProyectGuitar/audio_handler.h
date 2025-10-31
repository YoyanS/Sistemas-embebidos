#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <stdint.h>

// Declarar funciones
void setupAudio();
void reproducirAudio(String estadoActual);

// Declarar TODOS los audios
extern const uint8_t afinada_audio[];
extern const uint8_t apretar_audio[];
extern const uint8_t aflojar_audio[];

extern const uint32_t AFINADA_SIZE;
extern const uint32_t APRETAR_SIZE;
extern const uint32_t AFLOJAR_SIZE;

#endif