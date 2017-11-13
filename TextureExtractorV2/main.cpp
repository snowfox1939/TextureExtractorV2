
//  main.cpp
//  TextureExtractor
//
//  Created by Danil Luzin on 07/10/2017.
//  Copyright © 2017 Danil Luzin. All rights reserved.
//

#include <iostream>
#include "Timer.hpp"
#include "Mesh.hpp"
#include "TextureExtractor.hpp"
#include "Utils.h"
#include <numeric>



bool prepareMesh(Mesh & mesh,const std::string & objFilePath);

bool prepareViews( TextureExtractor & extractor, const std::string & cameraInfoPath, const std::string &  cameraListFilePath);

bool performViewSelection(TextureExtractor & extractor);

bool calculateDataCosts(TextureExtractor & extractor);

bool generateTexture(const std::string & newTexturePath, TextureExtractor & extractor, int width, int height);

void _renderViewsWithTexture(TextureExtractor & extractor);

bool justRender = true;

int main(int argc, const char * argv[]) {
    
    TextureExtractor extractor;
    Timer mainTimer;
    mainTimer.start();
    
    std::string objFilePath = "./resources/pig/pig_3_blender.obj";
    std::string cameraListFilePath = "resources/pig/list.txt";
    std::string cameraInfoPath = "resources/pig/bundle.rd.out";
    std::string newTexturePath = "resources/pig/derived/texture.ppm";
    std::string photoFolderPath = "resources/pig";
    
//    std::string objFilePath = "resources/slany/slany_blender_1.obj";
//    std::string cameraListFilePath = "resources/slany/list2.txt";
//    std::string cameraInfoPath = "resources/slany/bundle.rd.out";
//    std::string newTexturePath = "resources/pig/derived/texture.txt";
//    std::string photoFolderPath = "resources/slany";
    
    int textureWidth = 1000, textureHeight = 1000;


    Mesh mesh;
    bool meshIsOk = prepareMesh(mesh, objFilePath);
    if( !meshIsOk ){
        printBold(mainTimer.stopGetResults("\nExited with error"));
        return -1;
    }
    
    extractor.setPhotoFolderPath(photoFolderPath);
    extractor.setMesh( mesh );
    bool viewsOK;
    viewsOK = prepareViews( extractor, cameraInfoPath, cameraListFilePath );
    if( !viewsOK ){
        printBold(mainTimer.stopGetResults("\nExited with error"));
        return -1;
    }
    
    {
        //TODO:remove
        if(justRender){
            _renderViewsWithTexture(extractor);
            return 1;
        }
        {
        Bitmap bitmap;
        Bitmap texture("resources/pig/derived/texture.ppm");
//                std::vector<uint> photoSet={51};
//                std::vector<uint> photoSet={1,26,51};
        std::vector<uint> photoSet(extractor.numberOfViews());
        //                std::vector<uint> photoSet={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
        std::iota(photoSet.begin(),photoSet.end(),1);
        for(int t=0;t<photoSet.size();t++){
            std::cout<<"Rasterizing photos %"<<(float)t/photoSet.size()<<"\n";
            extractor.renderView(bitmap,texture, photoSet[t]);
            bitmap.toPPM("resources/pig/extract/pig_" + std::to_string(photoSet[t]) + ".ppm");
        }
        }
    }
    
    bool dataCostsOK;
    dataCostsOK = calculateDataCosts( extractor );
    if( !dataCostsOK ){
        printBold(mainTimer.stopGetResults("\nExited with error"));
        return -1;
    }
    
    
    bool viewSelectionOK;
    viewSelectionOK = performViewSelection(extractor);
    if( !viewSelectionOK ){
        printBold(mainTimer.stopGetResults("\nExited with error"));
        return -1;
    }
    
    bool textureOk;
    textureOk = generateTexture(newTexturePath, extractor, textureWidth, textureHeight);
    if( !textureOk ){
        printBold(mainTimer.stopGetResults("\nExited with error"));
        return -1;
    }
    
    printBold(mainTimer.stopGetResults( "\nTottal run time " ));
}

void _renderViewsWithTexture(TextureExtractor & extractor){
    {
        Bitmap bitmap;
        Bitmap texture("resources/pig/derived/texture.ppm");
        //        std::vector<uint> photoSet={};
        //        std::vector<uint> photoSet={1,26,51};
//        std::vector<uint> photoSet(extractor.numberOfViews());
        std::vector<uint> photoSet={1,2,3,44,5,46,7,8,9,30,31,12,13,22,51,16,50};
//        std::iota(photoSet.begin(),photoSet.end(),1);
        for(int t=0;t<photoSet.size();t++){
            std::cout<<"\rRasterizing photos %"<<(100*((float)t/photoSet.size()))<<"     "<<std::flush;
            extractor.renderView(bitmap,texture, photoSet[t]);
            bitmap.toPPM("resources/pig/extract/res/pig_" + std::to_string(photoSet[t]) + ".ppm");
        }
        std::cout<<"\rRasterizing photos %100      \n";
    }
}


bool generateTexture(const std::string & newTexturePath, TextureExtractor & extractor,int width, int height){
    Timer timer;
    timer.start();
    print("\nPerforming Texture Generation:\n");
    bool textureOk;
    textureOk = extractor.generateTexture(newTexturePath, width, height);
    if( !textureOk ){
        printBold(timer.stopGetResults( "\tTexture generation failed.: " ));
        return false;
    }
    print(timer.stopGetResults( "\tLables generated.: " ));
    return true;
}


bool performViewSelection(TextureExtractor & extractor){
    Timer timer;
    timer.start();
    print("\nPerforming Lable Assignment:\n");
    bool viewSelectionOK;
    viewSelectionOK = extractor.selectViews();
    if( !viewSelectionOK ){
        printBold(timer.stopGetResults( "\tLables generation failed.: " ));
        return false;
    }
    print(timer.stopGetResults( "\tLables generated.: " ));
    return true;
}


bool prepareMesh(Mesh & mesh,const std::string & objFilePath){
    Timer timer;
    timer.start();
    print("\nReading and Preparing the Mesh from obj File:\n");
    bool meshIsOk;
    meshIsOk = mesh.initialize(objFilePath);
    if( !meshIsOk ){
        printBold(timer.stopGetResults( "\tMesh inicialization failed.: " ));
        return false;
    }
    print(timer.stopGetResults( "\tMesh inicialized.: " ));
    return true;
}


bool prepareViews( TextureExtractor & extractor, const std::string & cameraInfoPath, const std::string &  cameraListFilePath){
    Timer timer;
    timer.start();
    std::cout <<"\nReading and Preparing Camera Views:\n";
    bool viewsAreOK;
    viewsAreOK = extractor.prepareViews( cameraInfoPath, cameraListFilePath);
    if( !viewsAreOK ){
        std::cout << timer.stopGetResults( "\tViews inicialization failed.: " );
        return false;
    }
    std::cout << timer.stopGetResults( "\tViews inicialized.: " );
    return true;
}


bool calculateDataCosts(TextureExtractor & extractor){
    Timer timer;
    timer.start();
    std::cout <<"\nCalculating Data Costs:\n";
    bool dataCostsOK;
    dataCostsOK = extractor.calculateDataCosts();
    if( !dataCostsOK ){
        std::cout << timer.stopGetResults( "\tData Costs Calculation failed.: " );
        return false;
    }
    std::cout << timer.stopGetResults( "\tData Costs Calculated.: " );
    return true;
}



//DEBUG





