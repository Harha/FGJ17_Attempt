#ifndef RESMANAGER_H
#define RESMANAGER_H

#include <string>
#include <map>

typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;
typedef struct _TTF_Font TTF_Font;
typedef struct _Mix_Music Mix_Music;
class Game;
class Level;

class ResManager
{
public:
	ResManager(Game * const game);
	~ResManager();
	SDL_Surface * const loadSurface(const std::string & filePath);
	SDL_Texture * const loadTexture(const std::string & filePath);
	TTF_Font * const loadFont(const std::string & filePath, uint32_t fontSize = 16);
	Mix_Music * const loadMusic(const std::string & filePath);
	Level * const loadLevel(const std::string & filePath, const std::string & name);
private:
	Game * const m_game;
	std::map<std::string, SDL_Surface *> m_surfaces;
	std::map<std::string, SDL_Texture *> m_textures;
	std::map<std::string, TTF_Font *> m_fonts;
	std::map<std::string, Mix_Music *> m_music;
	std::map<std::string, Level *> m_levels;
};

#endif // RESMANAGER_H
