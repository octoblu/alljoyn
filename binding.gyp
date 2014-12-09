{
  'variables': { 'target_arch%': 'x86' },
  'target_defaults': {
    'default_configuration': 'debug',
    'configurations': {
      'debug': {
        'defines': [ 'VARIANT=debug' ]
      },
      'release': {
        'defines': [ 'VARIANT=release' ]
      }
    },
    'include_dirs': [
      'gen',
      'gen/alljoyn',
      'alljoyn/alljoyn_core/inc',
      'alljoyn/alljoyn_core/inc/alljoyn',
      'alljoyn/alljoyn_core/router',
      'alljoyn/alljoyn_core/src',
      'alljoyn/common/inc',
      'alljoyn/services/services_common/cpp/inc',
      'alljoyn/services/about/cpp/inc',
      'alljoyn/services/notification/cpp/inc',
      "<!(node -e \"require('nan')\")"
    ],
    'cflags':[
      '-Wno-deprecated',
      '-Wno-ignored-qualifiers'
    ],
    'xcode_settings': {
        'OTHER_CFLAGS': [ 
          '-Wno-error=non-pod-varargs',
          '-Wno-ignored-qualifiers'
         ]
    },
    'conditions': [
      ['OS=="mac"', {
        'defines': [
          'QCC_OS_GROUP_POSIX',
          'QCC_OS_DARWIN'
        ],
        'include_dirs': [
          'alljoyn/alljoyn_core/router/posix',
        ],
        'link_settings': {
          'libraries': [
          '-lstdc++',
          '-lcrypto',
          '-lpthread'
          ]
        },
      }],
      ['OS=="win"', {
        'defines': [
          'QCC_OS_GROUP_WINDOWS',
          'WIN32_LEAN_AND_MEAN',
          'UNICODE'
        ],
        'include_dirs': [
          'alljoyn/alljoyn_core/router/windows',
        ],
        'link_settings': {
          'libraries': [
          '-lws2_32',
          '-lSecur32',
          '-lIphlpapi',
          '-lcrypt32',
          '-lNcrypt',
          '-lBcrypt'
          ]
        }
      }],
      ['OS=="linux"', {
        'defines': [
          'QCC_OS_GROUP_POSIX',
          'QCC_OS_LINUX'
        ],
        'include_dirs': [
          'alljoyn/alljoyn_core/router/posix',
        ],
        'link_settings': {
          'libraries': [
          '-lstdc++',
          '-lcrypto',
          '-lpthread',
          '-lrt'
          ]
        },
      }]
    ]
  },
  'targets': [
    {
      'target_name': 'node-alljoyn',
      'dependencies': [
        'alljoyn',
        'ajrouter'
      ],
      'conditions': [
        ['OS=="linux"', {
          'dependencies': [
            'ajnotification'
          ],
        }],
        ['OS=="mac"', {
          'dependencies': [
            'ajnotification'
          ],
        }],
      ],
      'sources': [
        '<!@(ls -1 src/*.cc)',
        'alljoyn/alljoyn_core/router/bundled/BundledRouter.cc'
      ]
    },
    {
      'target_name': 'alljoynstatus',
      'product_prefix': 'lib',
      'type': 'static_library',
      'sources': [
        'gen/alljoyn/version.cc',
        'gen/alljoyn/Status.cc'
      ],
    },
    {
      'target_name': 'alljoyn',
      'product_prefix': 'lib',
      'type': 'static_library',
      'dependencies': [
        'alljoynstatus'
      ],
      'sources': [
        'alljoyn/alljoyn_core/src/BaseClientTransport.cc',
        'alljoyn/alljoyn_core/src/AllJoynCrypto.cc',
        'alljoyn/alljoyn_core/src/AuthMechSRP.cc',
        'alljoyn/alljoyn_core/src/InterfaceDescription.cc',
        'alljoyn/alljoyn_core/src/Message_Parse.cc',
        'alljoyn/alljoyn_core/src/ProtectedAuthListener.cc',
        'alljoyn/alljoyn_core/src/SignatureUtils.cc',
        'alljoyn/alljoyn_core/src/AllJoynPeerObj.cc',
        'alljoyn/alljoyn_core/src/BusAttachment.cc',
        'alljoyn/alljoyn_core/src/CompressionRules.cc',
        'alljoyn/alljoyn_core/src/KeyExchanger.cc',
        'alljoyn/alljoyn_core/src/MethodTable.cc',
        'alljoyn/alljoyn_core/src/ProxyBusObject.cc',
        'alljoyn/alljoyn_core/src/SimpleBusListener.cc',
        'alljoyn/alljoyn_core/src/AllJoynStd.cc',
        'alljoyn/alljoyn_core/src/BusEndpoint.cc',
        'alljoyn/alljoyn_core/src/CredentialAccessor.cc',
        'alljoyn/alljoyn_core/src/KeyStore.cc',
        'alljoyn/alljoyn_core/src/MsgArg.cc',
        'alljoyn/alljoyn_core/src/RemoteEndpoint.cc',
        'alljoyn/alljoyn_core/src/Transport.cc',
        'alljoyn/alljoyn_core/src/AuthMechLogon.cc',
        'alljoyn/alljoyn_core/src/BusObject.cc',
        'alljoyn/alljoyn_core/src/DBusCookieSHA1.cc',
        'alljoyn/alljoyn_core/src/LocalTransport.cc',
        'alljoyn/alljoyn_core/src/NullTransport.cc',
        'alljoyn/alljoyn_core/src/SASLEngine.cc',
        'alljoyn/alljoyn_core/src/TransportList.cc',
        'alljoyn/alljoyn_core/src/AuthMechPIN.cc',
        'alljoyn/alljoyn_core/src/BusUtil.cc',
        'alljoyn/alljoyn_core/src/DBusStd.cc',
        'alljoyn/alljoyn_core/src/Message.cc',
        'alljoyn/alljoyn_core/src/PasswordManager.cc',
        'alljoyn/alljoyn_core/src/SessionOpts.cc',
        'alljoyn/alljoyn_core/src/XmlHelper.cc',
        'alljoyn/alljoyn_core/src/AuthMechRSA.cc',
        'alljoyn/alljoyn_core/src/ClientRouter.cc',
        'alljoyn/alljoyn_core/src/EndpointAuth.cc',
        'alljoyn/alljoyn_core/src/Message_Gen.cc',
        'alljoyn/alljoyn_core/src/PeerState.cc',
        'alljoyn/alljoyn_core/src/SignalTable.cc',
        'alljoyn/common/src/ASN1.cc',
        'alljoyn/common/src/BufferedSource.cc',
        'alljoyn/common/src/Crypto.cc',
        'alljoyn/common/src/Debug.cc',
        'alljoyn/common/src/IPAddress.cc',
        'alljoyn/common/src/Pipe.cc',
        'alljoyn/common/src/SocketStream.cc',
        'alljoyn/common/src/StreamPump.cc',
        'alljoyn/common/src/StringUtil.cc',
        'alljoyn/common/src/XmlElement.cc',
        'alljoyn/common/src/BigNum.cc',
        'alljoyn/common/src/CertificateECC.cc',
        'alljoyn/common/src/CryptoECC.cc',
        'alljoyn/common/src/GUID.cc',
        'alljoyn/common/src/KeyBlob.cc',
        'alljoyn/common/src/SLAPPacket.cc',
        'alljoyn/common/src/String.cc',
        'alljoyn/common/src/ThreadPool.cc',
        'alljoyn/common/src/BufferedSink.cc',
        'alljoyn/common/src/Config.cc',
        'alljoyn/common/src/CryptoSRP.cc',
        'alljoyn/common/src/IODispatch.cc',
        'alljoyn/common/src/Logger.cc',
        'alljoyn/common/src/SLAPStream.cc',
        'alljoyn/common/src/Stream.cc',
        'alljoyn/common/src/StringSource.cc',
        'alljoyn/common/src/Timer.cc',
        'alljoyn/common/src/Util.cc',
      ],
      'conditions': [
        ['OS=="win"', {
          'sources': [
            'alljoyn/common/os/windows/Environ.cc',
            'alljoyn/common/os/windows/FileStream.cc',
            'alljoyn/common/os/windows/OSLogger.cc',
            'alljoyn/common/os/windows/Socket.cc',
            'alljoyn/common/os/windows/Thread.cc',
            'alljoyn/common/os/windows/UARTStream.cc',
            'alljoyn/common/os/windows/IfConfig.cc',
            'alljoyn/common/os/windows/osUtil.cc',
            'alljoyn/common/os/windows/Event.cc',
            'alljoyn/common/os/windows/Mutex.cc',
            'alljoyn/common/os/windows/RWLock.cc',
            'alljoyn/common/os/windows/SslSocket.cc',
            'alljoyn/common/os/windows/time.cc',
            'alljoyn/common/os/windows/utility.cc',
            'alljoyn/common/crypto/cng/CryptoAES.cc',
            'alljoyn/common/crypto/cng/CryptoHash.cc',
            'alljoyn/common/crypto/cng/CryptoRSA.cc',
            'alljoyn/common/crypto/cng/CryptoRand.cc',
            'alljoyn/alljoyn_core/src/windows/ClientTransport.cc'
          ]
        }],
        ['OS=="mac"', {
          'sources': [
            'alljoyn/common/os/posix/Environ.cc',
            'alljoyn/common/os/posix/FileStream.cc',
            'alljoyn/common/os/posix/IfConfigLinux.cc',
            'alljoyn/common/os/posix/OSLogger.cc',
            'alljoyn/common/os/posix/Socket.cc',
            'alljoyn/common/os/posix/Thread.cc',
            'alljoyn/common/os/posix/UARTStreamDarwin.cc',
            'alljoyn/common/os/posix/atomic.cc',
            'alljoyn/common/os/posix/osUtil.cc',
            'alljoyn/common/os/posix/Event.cc',
            'alljoyn/common/os/posix/IfConfigDarwin.cc',
            'alljoyn/common/os/posix/Mutex.cc',
            'alljoyn/common/os/posix/RWLock.cc',
            'alljoyn/common/os/posix/SslSocket.cc',
            'alljoyn/common/os/posix/UARTStreamLinux.cc',
            'alljoyn/common/os/posix/time.cc',
            'alljoyn/common/crypto/openssl/CryptoAES.cc',
            'alljoyn/common/crypto/openssl/CryptoHash.cc',
            'alljoyn/common/crypto/openssl/CryptoRSA.cc',
            'alljoyn/common/crypto/openssl/CryptoRand.cc',
            'alljoyn/common/crypto/openssl/OpenSsl.cc',
            'alljoyn/alljoyn_core/src/darwin/ClientTransport.cc'
          ]
        }],
        ['OS=="linux"', {
          'sources': [
            'alljoyn/common/os/posix/Environ.cc',
            'alljoyn/common/os/posix/FileStream.cc',
            'alljoyn/common/os/posix/IfConfigLinux.cc',
            'alljoyn/common/os/posix/OSLogger.cc',
            'alljoyn/common/os/posix/Socket.cc',
            'alljoyn/common/os/posix/Thread.cc',
            'alljoyn/common/os/posix/UARTStreamDarwin.cc',
            'alljoyn/common/os/posix/atomic.cc',
            'alljoyn/common/os/posix/osUtil.cc',
            'alljoyn/common/os/posix/Event.cc',
            'alljoyn/common/os/posix/IfConfigDarwin.cc',
            'alljoyn/common/os/posix/Mutex.cc',
            'alljoyn/common/os/posix/RWLock.cc',
            'alljoyn/common/os/posix/SslSocket.cc',
            'alljoyn/common/os/posix/UARTStreamLinux.cc',
            'alljoyn/common/os/posix/time.cc',
            'alljoyn/common/crypto/openssl/CryptoAES.cc',
            'alljoyn/common/crypto/openssl/CryptoHash.cc',
            'alljoyn/common/crypto/openssl/CryptoRSA.cc',
            'alljoyn/common/crypto/openssl/CryptoRand.cc',
            'alljoyn/common/crypto/openssl/OpenSsl.cc',
            'alljoyn/alljoyn_core/src/posix/ClientTransport.cc'
          ]
        }]
      ]
    },
    {
      'target_name': 'ajnotification',
      'product_prefix': 'lib',
      'type': 'static_library',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources': [
            '<!@(ls -1 alljoyn/services/about/cpp/src/*.cc)',
            '<!@(ls -1 alljoyn/services/notification/cpp/src/*.cc)',
            '<!@(ls -1 alljoyn/services/services_common/cpp/src/*.cc)'
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            '<!@(ls -1 alljoyn/services/about/cpp/src/*.cc)',
            '<!@(ls -1 alljoyn/services/notification/cpp/src/*.cc)',
            '<!@(ls -1 alljoyn/services/services_common/cpp/src/*.cc)'
          ],
        }],
      ],
    },
    {
      'target_name': 'ajrouter',
      'product_prefix': 'lib',
      'type': 'static_library',
      'dependencies': [
        'alljoynstatus',
        'alljoyn'
      ],
      'sources': [
        'alljoyn/alljoyn_core/router/bundled/BundledRouter.cc',
        'alljoyn/alljoyn_core/router/AllJoynDebugObj.cc',
        'alljoyn/alljoyn_core/router/Bus.cc',
        'alljoyn/alljoyn_core/router/ConvertUTF.cc',
        'alljoyn/alljoyn_core/router/DaemonSLAPTransport.cc',
        'alljoyn/alljoyn_core/router/TCPTransport.cc',
        'alljoyn/alljoyn_core/router/UDPTransport.cc',
        'alljoyn/alljoyn_core/router/AllJoynObj.cc',
        'alljoyn/alljoyn_core/router/BusController.cc',
        'alljoyn/alljoyn_core/router/DBusObj.cc',
        'alljoyn/alljoyn_core/router/RuleTable.cc',
        'alljoyn/alljoyn_core/router/ArdpProtocol.cc',
        'alljoyn/alljoyn_core/router/ConfigDB.cc',
        'alljoyn/alljoyn_core/router/DaemonRouter.cc',
        'alljoyn/alljoyn_core/router/NameTable.cc',
        'alljoyn/alljoyn_core/router/SessionlessObj.cc',
        'alljoyn/alljoyn_core/router/VirtualEndpoint.cc',
        'alljoyn/alljoyn_core/router/PermissionMgr.cc',
        'alljoyn/alljoyn_core/router/ns/IpNameService.cc',
        'alljoyn/alljoyn_core/router/ns/IpNameServiceImpl.cc',
        'alljoyn/alljoyn_core/router/ns/IpNsProtocol.cc',
        'alljoyn/alljoyn_core/router/policydb/PolicyDB.cc',
        'alljoyn/alljoyn_core/router/packetengine/Packet.cc',
        'alljoyn/alljoyn_core/router/packetengine/PacketEngine.cc',
        'alljoyn/alljoyn_core/router/packetengine/PacketEngineStream.cc',
        'alljoyn/alljoyn_core/router/packetengine/PacketPool.cc'
      ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [
            'alljoyn/alljoyn_core/router/darwin/DaemonTransport.cc',
            'alljoyn/alljoyn_core/router/posix/PermissionMgr2.cc',
            'alljoyn/alljoyn_core/router/posix/Socket.cc',
          ]
        }],
        ['OS=="win"', {
          'sources': [
            'alljoyn/alljoyn_core/router/windows/PermissionMgr2.cc',
            'alljoyn/alljoyn_core/router/windows/Socket.cc',
            'alljoyn/alljoyn_core/router/windows/DaemonTransport.cc'
          ]
        }],
        ['OS=="linux"', {
          'sources': [
            'alljoyn/alljoyn_core/router/posix/PermissionMgr2.cc',
            'alljoyn/alljoyn_core/router/posix/Socket.cc',
            'alljoyn/alljoyn_core/router/posix/DaemonTransport.cc'
          ]
        }]
      ]
    },
    {
      'target_name': 'allchat',
      'type': 'executable',
      'dependencies': [
        'alljoynstatus',
        'alljoyn',
        'ajrouter'
      ],
      'sources': [
        'src/test_allchat.cc',
        'alljoyn/alljoyn_core/router/bundled/BundledRouter.cc'
      ],
    }
  ]
}