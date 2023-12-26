#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "asset.h"

void initAssets(Assets* assets){
    assets->asset_textures = NULL;
    assets->count = 0;
    assets->capacity = 0;
}

void addAsset(Assets* assets, Asset asset){
        TraceLog(LOG_INFO, "Reallocating assets to %d", assets->capacity);
    if(assets->count == assets->capacity){
        assets->capacity = assets->capacity == 0 ? 4 : assets->capacity * 2;
        TraceLog(LOG_INFO, " ASSET TEXTURES %zu", sizeof(Texture2D));
        assets->asset_textures = realloc(assets->asset_textures, assets->capacity * sizeof(Texture2D));
    }
    assets->asset_textures[assets->count] = asset;
    assets->count++;
}

Asset* getAsset(Assets* assets, const char* name){
    for(int i =0; i < assets->count; i++){
        if(asset_name(assets->asset_textures[i]) == name){
            return &assets->asset_textures[i];
        }
    }
}

void* removeAsset(Assets* assets, const char* name){
    for(int i =0; i < assets->count; i++){
        if(asset_name(assets->asset_textures[i]) == name){
            _dropAsset(&assets->asset_textures[i]);
            for(int j = i; j < assets->count - 1; j++){
                assets->asset_textures[j] = assets->asset_textures[j + 1];
            }
            assets->count--;
        }
    }
}
static void _dropAsset(Asset* asset){
    switch(asset_type_ptr(asset)){
        case TEXTURE:
            UnloadTexture(*(Texture2D*)(asset->ptr));
            break;
        default:
            break;
    }
}

void unloadAssets(Assets* assets){
    for(int i = 0; i < assets->count; i++){
        _dropAsset(&assets->asset_textures[i]);
    }
    free(assets->asset_textures);
}

