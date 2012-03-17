#!/usr/bin/env python

from distutils.core import setup, Extension
from datetime import datetime

# setup the comptime.h file
f = open('comptime.h', 'w')
f.write( """#define COMPILE_TIME "%s" """ % datetime.now().strftime( '+%Y %b %e %T %Z' ) )
f.close()

sdl_c=['oshw-sdl/ccicon.c',    'oshw-sdl/sdlerr.c',  'oshw-sdl/sdloshw.c',  'oshw-sdl/sdlsfx.c',
       'oshw-sdl/oshwbind.c',  'oshw-sdl/sdlin.c',   'oshw-sdl/sdlout.c',   'oshw-sdl/sdltext.c',
      ]

qt_cpp=['oshw-qt/CCMetaData.cpp',     'oshw-qt/oshwbind.cpp',      'oshw-qt/TWDisplayWidget.cpp',  
        'oshw-qt/TWProgressBar.cpp',  'oshw-qt/moc_TWMainWnd.cpp', 'oshw-qt/TWApp.cpp',     
        'oshw-qt/TWMainWnd.cpp',      'oshw-qt/_sdlsfx.c',
       ]

generic=['generic/generic.c',  'generic/_in.cpp',  'generic/tile.c',  'generic/timer.c']

tileworld = Extension('tworld',
                       #define_macros = [('DUMMY_SFX_IMPL', '1')],
                       sources = ['cmdline.c',  'err.c',       'help.c',
                                  'play.c',      'random.c',  
                                  'score.c',    'solution.c',  'unslist.c',
                                  'encoding.c', 'fileio.c',    'lxlogic.c',
                                  'mslogic.c',  'py_binding.c','res.c',
                                  'series.c',   'tworld.c',
                                  ] + qt_cpp + generic,
                       include_dirs= ['oshw-qt', '/usr/include/SDL', '/usr/include', '/usr/include/QtCore', '/usr/include/QtGui', '/usr/include/QtXml'],
                       libraries = ['QtCore', 'QtGui', 'QtXml', 'SDL' ] 
                      )

setup (name = 'Chips Challage Planner',
       version = '0.1',
       description = 'The tile world to planner package',
       long_description = """
           2012
       """,
       author='Harry Bullen',
       author_email='hbullen@gmail.com',
       #packages=['tileworld'],
       ext_modules = [tileworld]
       )
