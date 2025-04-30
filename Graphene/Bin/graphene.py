#!/usr/bin/python

import sys,os
arguments=''
run=False
fileName=''

initSetting='''/*
 *
 *  Setting for Graphene UI Framework
 *
 */

#define GLOBAL_ICON_FONT "Graphene/icons.ttf"
#define GLOBAL_TEXT_FONT "Graphene/Fonts/JetBrainsMono-Thin.ttf"

#define GLOBAL_TEXT_SIZE 38
#define GLOBAL_ICON_SIZE 35
'''

initCode='''

#include"Graphene.cpp"
#include"Widgets/Button.cpp"
#include"Widgets/NavigationDrawer.cpp"
#include"Widgets/Info.cpp"

Button_Released(showNavDraw){
    NavigationDrawer* navDraw=static_cast<NavigationDrawer*>(ptr);
    navDraw->toggle();
}

Button_Released(openInfo){
    Info* info=static_cast<Info*>(ptr);
    info->show();
}


Button_Released(info_dismiss){
    Info* info=static_cast<Info*>(ptr);
    info->hide();
}

Button_Released(exitButton_exitApp){
    Graphene::quit();
}

int main(){
    Graphene graphene("Demo App");

    Widget storage;
    storage.setXY(0.0,0.0).setWH(1.0,1.0);

    IconButton iconButton(MENU);
    iconButton.setXY(0.35,0.47).setWH(0.3,0.05);

    NavigationDrawer navDraw;
    navDraw.dismissOnExternalTouch().setRange(-0.7f,0.0f,0.0f,0.0f).setWH(0.7f,1.0f);

    IconTextButton navDrawButton(ICON_SY,"More Info",GLOBAL_ICON_SIZE,GLOBAL_TEXT_SIZE);

    IconButton exitButton(EXIT_TO_APP);

    navDrawButton.alignIconText().alignTextLeft();
    navDrawButton.setColor(themes::surface).setXY(0.0,0.9).setWH(1.0,0.1);

    exitButton.setColor(themes::surface).setXY(0.0,0.0).setWH(1.0,0.1);

    iconButton.setPointer(static_cast<void*>(&navDraw));
    iconButton.setOnReleasedCallback(showNavDraw);

    exitButton.setOnReleasedCallback(exitButton_exitApp);
    Info info;
    info.ok.setPointer(static_cast<void*>(&info)).setOnReleasedCallback(info_dismiss);
    info.close.setPointer(static_cast<void*>(&info)).setOnReleasedCallback(info_dismiss);
    info.dismissOnExternalTouch().setXY(0.2,0.2).setWH(0.6,0.6);

    info.setText(R"(This is a demo application for high performance graphene ui,
                 written by sy.)");

    navDrawButton.setPointer(static_cast<void*>(&info)).setOnReleasedCallback(openInfo);

    navDraw.add(&navDrawButton);
    navDraw.add(&exitButton);
    storage.add(&iconButton);
    storage.add(&navDraw);
    storage.add(&info);

    // themes::setDarkMode();

    graphene.run(&storage);
}
'''

if(not os.path.isdir("Output")):
    os.mkdir("Output")

for i in sys.argv:
    if(i=="init"):
        if(not os.path.isdir("Src")):
            os.mkdir("Src")
        if(not os.path.isfile("Src/App.cpp")):
            with open("Src/App.cpp","w") as fd:
                fd.write(initCode)
                pass
            with open("Src/Setting.h","w") as fd:
                fd.write(initSetting)
                pass
        print("Init done...")
        exit()
    elif(i=='run'):
        fileName="Src/App.cpp"
        run=True
    elif(i=='runDemo'):
        fileName="Graphene/Demo/App.cpp"
        run=True
        break
    elif(i.startswith("arg=")):
        arguments+=f" {i[4:]}"
    elif(i=='sanitizer'):
        arguments+=" -fsanitize=address,undefined,alignment"

if(run==True):
    os.system(f'g++ -g -o Output/output Src/App.cpp -lSDL2_image -lGL $(pkg-config --cflags sdl2 freetype2 --libs) -IGraphene/ -ISrc/ {arguments} && ./Output/output')

