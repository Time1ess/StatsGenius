{
    "targets": [
        {
            "target_name": "mac-genius-bindings",
            "headers": [
                # '<!@(ls -1 darwin/cpu_meter/*.hpp)',
                # '<!@(ls -1 darwin/third_party/**/*.hpp)',
                '<!@(ls -1 darwin/*.hpp)',
                '<!@(ls -1 *.hpp)',
            ],
            "sources": [
                # '<!@(ls -1 darwin/cpu_meter/*.cpp)',
                # '<!@(ls -1 darwin/third_party/**/*.cpp)',
                '<!@(ls -1 darwin/*.cpp)',
                '<!@(ls -1 *.cpp)',
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "libraries": [
                # "-framework IOKit",
            ],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    }
                }]
            ]
        }
    ]
}
