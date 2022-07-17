struct inputs{
    char path[256];
    int a;
    int b;
};


program PARTB{
    version PARTB_VERS{
        string execute(inputs)=1;
    }=1;
}=0x12345678;