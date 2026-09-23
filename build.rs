fn main() {
    cc::Build::new()
        .file("runtime_c/doxrain_engine.c")
        .include("runtime_c/include")
        .warnings(true)
        .compile("doxrain_engine");
    println!("cargo:rerun-if-changed=runtime_c/doxrain_engine.c");
    println!("cargo:rerun-if-changed=runtime_c/include/doxrain_engine.h");
}
