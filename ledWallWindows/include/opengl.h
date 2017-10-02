#ifndef OPENGL_H
#define OPENGL_H
#include <SDL/SDL.h>

void setup_opengl( int width, int height );
void draw_screen( void );
void process_events( void );
void handle_key_down( SDL_keysym* keysym );
void quit_tutorial( int code );

#endif // OPENGL_H
