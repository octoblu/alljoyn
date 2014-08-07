{
  'targets': [
    {
      'target_name': 'node-alljoyn',
      'dependencies': [
        'alljoyn.gyp:alljoyn'
      ],
      'defines': [
        'QCC_OS_GROUP_POSIX',
        'QCC_OS_DARWIN'
      ],
      'include_dirs': [
        'gen',
        'gen/alljoyn',
        'alljoyn/alljoyn_core/inc',
        'alljoyn/alljoyn_core/inc/alljoyn',
        'alljoyn/common/inc',
        "<!(node -e \"require('nan')\")"
      ],
      'sources': [
        'src/bindings.cc',
        'src/InterfaceWrapper.cc',
        'src/BusConnection.cc'
      ]
    }
  ]
}