Pour faire marcher la biblioth�que Spout j'ai du :
installer la SDL (pour avoir la console j'ai comment� la ligne #include <SDLmain.h> dans SDL.h et je n'ai pas link� avec SDLmain)
donc link avec SDL.dll et avant mingw32
opengl => link avec opengl32, glu32, gdi32
link avec spoutLibrary pour qu'elle soit disponible

dll : sdl et spout
