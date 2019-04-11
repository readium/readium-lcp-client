{
  'includes': [
    'filenames.gypi'
  ],
  'variables': {

  },
  'target_defaults': {
    'include_dirs': [

    ],
    'cflags': [
      '-w',
      '-fPIC',
      '-fvisibility=hidden',
      '-g', # Debug mode
    ],
    'defines': [
      'BUILDING_EPUB3'
    ]
  },
  'targets': [
    {
      'target_name': 'lcp_content_filter',
      'type': 'static_library',
      'cflags_cc': [
        '-std=c++11'
      ],
      'sources': [
        '<@(lcp_content_filter_sources)'
      ]
    },
    {
      'target_name': 'lcp_client_lib',
      'type': 'static_library',
      'dependencies': [
        'cryptopp',
        'zip_lib',
        'time64'
      ],
      'include_dirs': [
        '<(third_party_dir)'
      ],
      'cflags_cc': [
        '-std=c++11',
        '-frtti',
        '-fexceptions',
      ],
      'sources': [
        '<@(lcp_client_lib_sources)'
      ]
    },
    {
      'target_name': 'cryptopp',
      'type': 'static_library',
      'cflags_cc': [
        '-std=c++11',
        '-fpermissive',
        '-frtti',
        '-fexceptions',
      ],
      'sources': [
        '<@(cryptopp_sources)'
      ]
    },
    {
      'target_name': 'zip_lib',
      'type': 'static_library',
      'dependencies': [
        'zlib',
        'bzip2'
      ],
      'cflags_cc': [
        '-std=c++11',
        '-fpermissive',
        '-frtti',
        '-fexceptions',
        '-DZLIB_ONLY'
      ],
      'sources': [
        '<@(zip_lib_sources)'
      ]
    },
    {
      'target_name': 'zlib',
      'type': 'static_library',
      'sources': [
        '<@(zlib_sources)'
      ]
    },
    {
      'target_name': 'bzip2',
      'type': 'static_library',
      'sources': [
        '<@(bzip2_sources)'
      ]
    },
    {
      'target_name': 'time64',
      'type': 'static_library',
      'sources': [
        '<@(time64_sources)'
      ]
    }
  ],
  'conditions': [
    ['OS=="mac"', {
        'target_defaults': {
          'defines': [
            'LIBXML_THREAD_ENABLED',
            'CRYPTOPP_DISABLE_ASM',
            'ZLIB_ONLY'
          ],
          'cflags_cc': [
            #'-std=c++0x',
            #'-stdlib=libc++',
            #'-Wtautological-pointer-compare',
            #'-Wc++11-extensions'
          ],
          'xcode_settings': {
            'GCC_TREAT_WARNINGS_AS_ERRORS': 'NO',
            #'MACOSX_DEPLOYMENT_TARGET': '10.8',
            'CLANG_CXX_LANGUAGE_STANDARD': 'gnu++0x',
				    'CLANG_CXX_LIBRARY': 'libc++',
            'WARNING_CFLAGS': [
              '-Wno-unknown-warning-option',
              '-Wno-parentheses-equality',
              '-Wno-unused-function',
              '-Wno-sometimes-uninitialized',
              '-Wno-pointer-sign',
              '-Wno-sign-compare',
              '-Wno-string-plus-int',
              '-Wno-unused-variable',
              '-Wno-deprecated-declarations',
              '-Wno-return-type',
              '-Wno-gnu-folding-constant',
              '-Wno-shift-negative-value',
              '-Wno-tautological-pointer-compare',
              '-Wno-inconsistent-missing-override',
              '-Wno-empty-body',
              '-Wno-uninitialized',
              '-Wno-potentially-evaluated-expression'
            ]
          },
          'ldflags': [

          ],
          'link_settings': {
            'libraries': [

            ]
          }
        }
    }],
    ['OS=="linux"', {
        'target_defaults': {
          'defines': [
            #'LIBXML_THREAD_ENABLED',
            #'_GLIBCXX_USE_CXX11_ABI=0' # to avoid std::locale issue
          ],
          'cflags': [
            '-m64',
            '-march=x86-64',
          ],
          'cflags_cc': [
          ],
          'ldflags': [
            '-m64',
          ],
          'link_settings': {
            'libraries': [

            ]
          }
        }
    }],
    ['OS=="win"', {
        'variables': {
        },
        'target_defaults': {
          'defines': [
            'MSVS_VERSION_2015',
            'NDEBUG',
            'NOMINMAX',
            'WIN32'
          ],
            'msvs_disabled_warnings': [
              4530,
              4577,
              4028,
              4090
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'WarnAsError': 'false',
              },
            },
          'cflags': [
            '/EHsc'
          ],
          'cflags_cc': [
            # '-std=c++11',
            # '-fpermissive'
          ],
          'include_dirs': [
          ],
          'link_settings': {
            'library_dirs': [
            ],
            'libraries': [
            ]
          }
        }
    }],
  ]
}
