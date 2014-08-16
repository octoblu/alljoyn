{
  'targets': [
    {
      'target_name': 'node-alljoyn',
      'dependencies': [
        'alljoyn.gyp:alljoyn',
        'alljoyn.gyp:ajrouter'
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
        'alljoyn/alljoyn_core/router',
        'alljoyn/alljoyn_core/router/posix',
        'alljoyn/alljoyn_core/src',
        'alljoyn/common/inc',
        "<!(node -e \"require('nan')\")"
      ],
      'sources': [
        'src/bindings.cc',
        'src/InterfaceWrapper.cc',
        'src/BusConnection.cc',
        'src/BusListenerWrapper.cc',
        'src/BusListenerImpl.cc',
        'src/BusObjectWrapper.cc',
        'src/SessionPortListenerWrapper.cc',
        'src/SessionPortListenerImpl.cc',
        'alljoyn/alljoyn_core/router/bundled/BundledRouter.cc'
      ]
    }
  ]
}