set(LLVM_LINK_COMPONENTS
  Analysis
  AsmParser
  Core
  Support
)
add_llvm_fuzzer(vfabi-demangler-fuzzer
  vfabi-demangler-fuzzer.cpp
)
