{
  "targets": [
    {
      "target_name": "binding",
      "sources": [ "src/binding.cc", "src/inf.cc" ],
      "include_dirs": ["<!(node -e \"require('nan')\")"],
      "libraries": [ "Setupapi.lib" ]
    }
  ]
}
