Import("env")

# Build filesystem before building firmware
env.AddPreAction("build", env.VerboseAction("pio run -t buildfs", "Building SPIFFS/LittleFS image..."))

# Upload filesystem after uploading firmware
env.AddPostAction("upload", env.VerboseAction("pio run -t uploadfs", "Uploading SPIFFS/LittleFS image...")) 