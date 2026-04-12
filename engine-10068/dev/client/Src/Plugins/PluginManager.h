#pragma once

#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

class PluginManager
{
public:
	static void loadScript(const char* path, const char* name);
};

#endif