#ifndef ASSET_H
#define ASSET_H
#include "raylib.h"
#include "stdlib.h"
#include "string.h"

typedef struct AssetPath {
    const char* path;
    const char* name;
} AssetPath;

const char* getAssetPath(const char* name);

typedef enum AssetType {
    IMAGE, 
    TEXTURE,
    TEXTURE_IMAGE,
} AssetType;

typedef union AssetData {
    Image image;
    Texture2D texture;
} AssetData;

typedef struct Asset {
    AssetData item;
    const char* name;
    AssetType type;
} Asset;


typedef struct Assets {
    Asset* items;
    int count;
    int capacity;
} Assets;


#define asset_name(asset) (asset.name) 
#define asset_type(asset) (asset.type)

#define add_asset(assets, name, type, ptr) addAsset(assets, (Asset){ptr, name, type}
#define asset_get(assets, name, type) asset_ptr((*getAsset(assets, "flag")), Texture2D);

#define asset_type_ptr(asset) (asset->type)


Texture assets_tex_from_img(Assets* assets, const char* name);
Image assets_image(Assets* assets, const char* name);
Texture assets_texture(Assets* assets, const char* name);
void initAssets(Assets* assets);
void removeAsset(Assets* assets, const char* name);
static void _dropAsset(Asset* asset);
void unloadAssets(Assets* assets);

#endif // ASSET_H
