/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/
#include "face_feature.h"
int main(int, char**) { 
    featureDB parse_test;
    /******************/
    Mat feature_input;
    int age;
    string gender;
    std::vector<string> names;
    int flag0=0,flag1=0;
    vector<string>::iterator it;
    databaseinfo person_result;

    /********in*******/
    personinput testperson;
    testperson.name = "常山赵子龙";
    testperson.gender = "男";
    testperson.age = 27;
    testperson.id_seq = 897578;
    /******************/
    /********in*******/
    personinput testperson_1;
    testperson_1.name = "常山赵子龙";
    testperson_1.gender = "男";
    testperson_1.age = 27;
    testperson_1.id_seq = 897578;
    /******************/
     /********in*******/
    personinput testperson_2;
    testperson_2.name = "子龙";
    testperson_2.gender = "男";
    testperson_2.age = 27;
    testperson_2.id_seq = 897578;
    /******************/

    feature_input = imread("./data/1.jpg",1);
    cout<<"person count:"<<parse_test.face_feature_get_person_count()<<endl;
    flag0 = parse_test.face_feature_add_db(testperson_2,feature_input);
    if (flag0 < 0 || flag0 == 2)
        goto end;
     parse_test.face_feature_getnamelist_db(names);
    //test
    for(it=names.begin();it!=names.end();++it)
    {
        cout << "name:"<<*it<<endl;
    }
    // test end
    names.clear();
    parse_test.face_feature_delete_db(testperson);
    cout<<"after delete:::::::::"<<endl;
    parse_test.face_feature_getnamelist_db(names);
    //test
    for(it=names.begin();it!=names.end();++it)
    {
        cout << "name:"<<*it<<endl;
    }
    parse_test.face_feature_update_age_db(testperson_2,37);
    // test end
    flag1 = parse_test.face_feature_query_db(testperson_2,person_result);
    if (flag1 < 0)
        goto end;
    for (int i = 0; i < person_result.feature.size(); i++)
    {
        char showstr[100] ="database";
        sprintf(showstr,"feature_out_%d",i);
        imshow(showstr,person_result.feature[i]);
        waitKey(-1);
        cout << "final gender:"<<person_result.gender<<endl;
        cout << "final age:"<<person_result.age<<endl;
        cout << "final id:"<<person_result.id_p<<endl;
    }
end: cout<<"game over"<<endl;
 //parse_test.face_feature_clear_db();
}
