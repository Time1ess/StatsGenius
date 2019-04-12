{
    "targets": [
        {
            "target_name": "darwin_cpu_meter",
            "headers": [
                "darwin/cpu_data.hpp",
            ],
            "sources": [
                "darwin/cpu.cpp"
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
