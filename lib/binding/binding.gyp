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
            ]
        }
    ]
}
