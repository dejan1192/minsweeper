#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "asset.h"

AssetPath paths[] = {
    {"assets/flag.png", "flag"},
};

void initAssets(Assets* assets){
    assets->items = NULL;
    assets->count = 0;
    assets->capacity = 0;
}

const char* getAssetPath(const char* name) {
    for (int i = 0; i < sizeof(paths) / sizeof(AssetPath); i++) {
        if (strcmp(paths[i].name, name) == 0) {
            return paths[i].path;
        }
    }
    return NULL;
}

static void addAsset(Assets* assets, Asset asset){
   if(assets->count == assets->capacity){
        assets->capacity = assets->capacity == 0 ? 4 : assets->capacity * 2;
        assets->items = realloc(assets->items, sizeof(Asset) * assets->capacity);
    }
    assets->items[assets->count++] = asset; 
}

Texture assets_tex_from_img(Assets* assets, const char* name){
    for(int i = 0; i < assets->count; i++){
        if(assets->items[i].type == TEXTURE_IMAGE){
            if(strcmp(assets->items[i].name, name) == 0){

                return assets->items[i].item.texture;
            }
        }
    }
    Texture texImg = LoadTextureFromImage(assets_image(assets, name));
    Asset asset = (Asset) { .item.texture = texImg, .name = name, .type = TEXTURE_IMAGE }; 
    addAsset(assets, asset);
    return texImg;
}

Image assets_image(Assets* assets, const char* name){
    for(int i = 0; i < assets->count; i++){
        if(assets->items[i].type == IMAGE){
            if(assets->items[i].name == name){
                return assets->items[i].item.image;
            }
        }
    }
    Image img = LoadImage(name);
    Asset asset = (Asset) { .item.image = img, .name = name, .type = IMAGE }; 
    addAsset(assets, asset);
    return img;
}

Texture assets_texture(Assets* assets, const char* name){
    for(int i = 0; i < assets->count; i++){
        if(assets->items[i].type == TEXTURE){
            if(assets->items[i].name == name){
                return assets->items[i].item.texture;
            }
        }
    }
    Texture t = LoadTexture(name);
    Asset asset = (Asset) { .item.texture = t, .name = name, .type = TEXTURE }; 
    addAsset(assets, asset);
    return t;
}

void removeAsset(Assets* assets, const char* name){
    for(int i =0; i < assets->count; i++){
        if(asset_name(assets->items[i]) == name){
            _dropAsset(&assets->items[i]);
            for(int j = i; j < assets->count - 1; j++){
                assets->items[j] = assets->items[j + 1];
            }
            assets->count--;
        }
    }
}

static void _dropAsset(Asset* asset){
    switch(asset_type_ptr(asset)){
        case TEXTURE:
        case TEXTURE_IMAGE:
            UnloadTexture((Texture2D)(asset->item.texture));
            break;
        case IMAGE:
            UnloadImage((Image)(asset->item.image));
            break;
        default:
            break;
    }
}

void unloadAssets(Assets* assets){
    for(int i = 0; i < assets->count; i++){
        _dropAsset(&assets->items[i]);
    }
    free(assets->items);
}

