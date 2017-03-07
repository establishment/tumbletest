#define MakeCopyAndMove(ClassName)                    \
  public:                                             \
    ClassName(const ClassName&) = default;            \
    ClassName& operator=(const ClassName&) = default; \
                                                      \
    ClassName(ClassName&&) = default;                 \
    ClassName& operator=(ClassName&&) = default;

#define MakeMoveOnly(ClassName)                      \
  public:                                            \
    ClassName(const ClassName&) = delete;            \
    ClassName& operator=(const ClassName&) = delete; \
                                                     \
    ClassName(ClassName&&) = default;                \
    ClassName& operator=(ClassName&&) = default;

#define PermanentSingleton(ClassName)  \
  public:                              \
    static ClassName& GetSingleton() { \
        static ClassName entity;       \
        return entity;                 \
    }                                  \
    MakeMoveOnly(ClassName) private : ClassName();
