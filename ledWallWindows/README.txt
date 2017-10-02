Pour faire marcher la bibliothèque Spout j'ai du :
installer la SDL (pour avoir la console j'ai commenté la ligne #include <SDLmain.h> dans SDL.h et je n'ai pas linké avec SDLmain)
donc link avec SDL.dll et avant mingw32
opengl => link avec opengl32, glu32, gdi32
link avec spoutLibrary pour qu'elle soit disponible

dll : sdl et spout
