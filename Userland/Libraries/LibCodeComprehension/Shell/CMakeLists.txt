set(SOURCES
    ShellComprehensionEngine.cpp
)

serenity_lib(LibShellComprehension shellcomprehension)
target_link_libraries(LibShellComprehension LibCodeComprehension LibC)
