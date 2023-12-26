#ifndef ASSET_H
#define ASSET_H

typedef enum AssetType {
    TEXTURE,
    SOUND,
    MUSIC,
    FONT,
} AssetType;


typedef struct Asset {
    void* ptr;
    const char* name;
    AssetType type;
} Asset;

typedef struct Assets {
    Asset* asset_textures;
    int count;
    int capacity;
} Assets;


#define asset_name(asset) (asset.name) 
#define asset_ptr(asset, type) *(type*)(asset.ptr)
#define asset_type(asset) (asset.type)

#define asset_get(assets, name, type) asset_ptr((*getAsset(assets, "flag")), Texture2D);

#define asset_type_ptr(asset) (asset->type)


void initAssets(Assets* assets);
void addAsset(Assets* assets, Asset asset);
Asset* getAsset(Assets* assets, const char* name);
void* removeAsset(Assets* assets, const char* name);
static void _dropAsset(Asset* asset);
void unloadAssets(Assets* assets);

#endif // ASSET_H
