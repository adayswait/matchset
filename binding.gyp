{
  "targets": [
    {
      "target_name": "matchset",
      'sources': ["src/main.cc"],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")","src"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'cflags_cc': [ ],
      'cflags': [ ],
    }
  ]
}
