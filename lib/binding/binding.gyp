{
    "targets": [
        {
            "target_name": "darwin_cpu_meter",
            "headers": [
                "darwin/cpu_meter/cpu_statistics.hpp",
                "darwin/cpu_meter/cpu_usage.hpp",
                "darwin/cpu_meter/cpu_load.hpp",
                "darwin/cpu_meter/process.hpp",
            ],
            "sources": [
                "darwin/cpu_meter/cpu_statistics.cpp",
                "darwin/cpu_meter/cpu_usage.cpp",
                "darwin/cpu_meter/cpu_load.cpp",
                "darwin/cpu_meter/process.cpp",
                "darwin/cpu_meter/cpu_meter.cpp",
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
