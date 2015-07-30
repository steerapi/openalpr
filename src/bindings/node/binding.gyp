{
  "targets": [
    {
      "target_name": "nodealpr",
      "sources": [ "alpr.cc" ],
      "conditions": [
              ['OS=="linux"', {
                'cflags': [
                  '-I/usr/include/'
                ],
                'ldflags': [
                  '-L/usr/lib/'
                ],
                'libraries': [
                  '-lopenalpr'
                ]
              }] 
            ]
    }
  ]
}