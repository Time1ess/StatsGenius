{
    "targets": [
        {
            "target_name": "darwin_cpu_meter",
            "headers": [
                '<!@(ls -1 darwin/cpu_meter/*.hpp)',
            ],
            "sources": [
                '<!@(ls -1 darwin/cpu_meter/*.cpp)',
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
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
