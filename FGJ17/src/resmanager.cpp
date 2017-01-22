#include "resmanager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "game.h"
#include "display.h"
#include "macros.h"

ResManager::ResManager(Game * const game) :
	m_game(game),
	m_surfaces(),
	m_textures()
{

}

ResManager::~ResManager()
{
	for (auto &entry : m_music)
		Mix_FreeMusic(entry.second);

	for (auto &entry : m_fonts)
		TTF_CloseFont(entry.second);

	for (auto &entry : m_surfaces)
		SDL_FreeSurface(entry.second);

	for (auto &entry : m_textures)
		SDL_DestroyTexture(entry.second);
}

SDL_Surface * const ResManager::loadSurface(const std::string & filePath)
{
	if (m_surfaces.count(filePath) == 0)
	{
		m_surfaces[filePath] = IMG_Load(filePath.c_str());

		if (m_surfaces[filePath] == NULL)
		{
			m_surfaces.erase(filePath);
			ERR("ResManager: Error loading surface (" << filePath << " into memory.");
			return nullptr;
		}

		LOG("ResManager: Loaded surface (" << filePath << ") into memory.");
	}

	return m_surfaces[filePath];
}

SDL_Texture * const ResManager::loadTexture(const std::string & filePath)
{
	if (m_textures.count(filePath) == 0)
	{
		m_textures[filePath] = SDL_CreateTextureFromSurface(m_game->getDisplay()->getRenderer(), loadSurface(filePath));

		if (m_textures[filePath] == NULL)
		{
			m_textures.erase(filePath);
			ERR("ResManager: Error loading texture (" << filePath << " into memory.");
			return nullptr;
		}

		LOG("ResManager: Loaded texture (" << filePath << ") into memory.");
	}

	return m_textures[filePath];
}

TTF_Font * const ResManager::loadFont(const std::string & filePath, uint32_t fontSize)
{
	if (m_fonts.count(filePath) == 0)
	{
		m_fonts[filePath] = TTF_OpenFont(filePath.c_str(), fontSize);

		if (m_fonts[filePath] == NULL)
		{
			m_fonts.erase(filePath);
			ERR("ResManager: Error loading font (" << filePath << " into memory.");
			return nullptr;
		}

		LOG("ResManager: Loaded font (" << filePath << ") into memory.");
	}

	return m_fonts[filePath];
}

Mix_Music * const ResManager::loadMusic(const std::string & filePath)
{
	if (m_music.count(filePath) == 0)
	{
		m_music[filePath] = Mix_LoadMUS(filePath.c_str());

		if (m_music[filePath] == NULL)
		{
			m_music.erase(filePath);
			ERR("ResManager: Error loading module (" << filePath << " into memory.");
			return nullptr;
		}

		LOG("ResManager: Loaded module (" << filePath << ") into memory.");
	}

	return m_music[filePath];
}
