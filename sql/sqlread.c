#include "../sql/csql.h"


/*
 *author:liuyang
 *date  :2017/3/19
 *detail:begin to read host image name
 *return 1:success,0:failed
 */
 int read_host_image_name(char **image_abspath){
    //host name
    char hostName[32];
    long int hostId;
    unsigned long long count;
    int i;
    int serverId;
    MYSQL *my_conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    my_conn=mysql_init(NULL);
    if(!mysql_real_connect(my_conn,"127.0.0.1","root","","detect",0,NULL,0)) //连接detect数据库
    {
        printf("Connect Error!n");
        exit(1);
    }
    /**************************************************check whether the host exist in database****************************************************/
    //read server host name and host id
    if(gethostname(hostName,sizeof(hostName))){
        perror("gethostname!");
        return -1;
    }
    hostId=gethostid();
    //查询数据库中是否有该服务器
    char strsql[256];
    sprintf(strsql,"select servers.id as sid,servers.IP as sip,servers.status as sstatus from servers where serverNumber=%d and name='%s'",hostId,hostName);
    //printf("%s",strsql);
    if(mysql_query(my_conn,strsql)) //连接baseImages表
    {
        printf("Query Error,query server failed!n");
        return -1;
    }
    res=mysql_store_result(my_conn); //取得表中的数据并存储到res中,mysql_use_result
    count=mysql_num_rows(res);
    if(count<=0){//
        printf("\n error:the server not exist!");
        return 0;
    }else if(count>1){
        printf("\n error:the server id repeat!");
        return -2;
    }else{
        row=mysql_fetch_row(res);//打印结果
        int host_status=atoi(row[2]);
        //host status
        if(host_status!=1){
            printf("\n the host stop detecting!");
            return -3;
        }
        serverId=atoi(row[0]);
        printf("\n host_status:%d,serverId:%d",host_status,serverId);
    }
    /**************************************************check whether the host exist in database***end*************************************************/
    /**************************************************find the images in the host*******************************************************/
    sprintf(strsql,"select baseImages.absPath,baseImages.status \
            from servers join serverImages join baseImages      \
            where servers.id=%d and servers.id=serverImages.serverId and serverImages.baseImageId=baseImages.id",serverId);
    if(mysql_query(my_conn,strsql)){
        printf("Query Error,query server images failed!n");
        return -1;
    }
    res=mysql_store_result(my_conn);
    count=mysql_num_rows(res);
    printf("\n count:%d",count);
    if(count<=0){
        printf("\n no images in the host!");
        return -4;
    }
    char *baseImages[count];
    //baseImages=malloc(count * sizeof(char *));
    i=0;
    while((row=mysql_fetch_row(res))){
        baseImages[i]=row[0];
        //baseImages[i]=strdup(baseImages[i]);
        printf("\n the image path:%s",baseImages[i]);
        i++;
    }

    /**************************************************find the images in the host***end*************************************************/
    //test mysql_fetch_fields,mysql_num_fields
//    int num_fields = mysql_num_fields(res);
//    MYSQL_FIELD *fields = mysql_fetch_fields(res);
//    for(i = 0; i < num_fields; i++)
//    {
//       printf("Field %u is %s\n", i, fields[i].name);
//    }
//    int num_fields = mysql_num_fields(res);
//    while ((row = mysql_fetch_row(res)))
//    {
//       unsigned long *lengths;
//       lengths = mysql_fetch_lengths(res);
//       for(i = 0; i < num_fields; i++)
//       {
//           printf("[%.*s] ", (int) lengths[i],
//                  row[i] ? row[i] : "NULL");
//       }
//       printf("\n");
//    }



    return 1;
 }