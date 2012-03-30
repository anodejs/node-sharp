{ 
  'targets': [
    {
      'target_name': 'Sharp',
      'type': 'static_library',
      'sources': [ 'Sharp/MatchType.cpp',
                  'Sharp/SharpAddon.cpp',
                  'Sharp/SharpLibHelper.cpp',
                  'Sharp/v8external.cpp',
                  'Sharp/v8function.cpp',
                  'Sharp/v8value.cpp',
                  'Sharp/v8wrap.cpp',
                  'Sharp/WrapAssembly.cpp',
                  'Sharp/WrapBase.cpp',
                  'Sharp/WrapInstance.cpp'
                   ] ,
      'libraries': ['-lm'],
      'conditions': [
       ['OS=="win"', {
         'cflags': ['/clr']
        }]
      ],
    }
  ]
}