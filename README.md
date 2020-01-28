## Building

1. Patch BUILD.gn using BUILD.gn.patch
2. Copy over dharmafuzz.cc and python_bridge.cc over to v8/test/fuzzer
3. Create a v8 build config using dharmafuzz.gn
4. Build the shell using `ninja -C out/dharmafuzz/ v8_dharmafuzz`

## Running

`PYTHONPATH=. LIBFUZZER_PYTHON_MODULE=pymodules.dharma_generator /home/amar/workspace/v8/out/dharmafuzz/v8_dharmafuzz --allow-natives-syntax --expose-gc corpus -rss_limit_mb=8510 -timeout=1200 -max_len=20480 -jobs=10000 -workers=12`

## Refrences

[dharma](https://github.com/MozillaSecurity/dharma)

[libfuzzer-python-bridge](https://github.com/MozillaSecurity/libfuzzer-python-bridge)