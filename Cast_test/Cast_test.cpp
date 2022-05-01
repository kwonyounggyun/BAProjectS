// Cast_test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

/*
* 1000만번 기준으로 dynamic_cast 와 static_cast 10배이상 시간차이가 발생
* 중간에 잇는 녀석들 골라내는것도 그냥 TYPE 체크같은거 잇는게 훨씬 빠르네...
*/

#include <iostream>
#include <Windows.h>
#include <vector>

static int nomal = 0;
static int smart = 0;
static int super = 0;

enum type
{
    NOMAL,
    SUPER,
    SMART
};

class BaseStudent
{
protected:
    double _homework_count;
    type _type;

public:
    BaseStudent() :_homework_count(0) {};
    virtual ~BaseStudent() = 0;

    virtual void Homework()
    {
        _homework_count += 1;
    }

    bool TypeCheck(type type)
    {
        return type == _type;
    }
};

class Student : public BaseStudent
{
public:
    Student() 
    {
        _type = NOMAL;
    }
    virtual ~Student() 
    {
        nomal++;
    }
    
    virtual void Homework()
    {
        _homework_count += 1;
    }
};

class SuperStudent : public BaseStudent
{
private:
    double _work_count;
public:
    SuperStudent() : _work_count(0) 
    {
        _type = SUPER;
    }
    virtual ~SuperStudent() 
    {
        super++;
    }

    virtual void Homework()
    {
        _homework_count += 1;
        _work_count += 1;
    }
};

class SmartStudent : public BaseStudent
{
private:
    double _smart_work;
public:
    SmartStudent() : _smart_work(0) 
    {
        _type = SMART;
    }
    virtual ~SmartStudent() 
    {
        smart++;
    }
    virtual void Homework()
    {
        _homework_count += 1;
        _smart_work += 1;
    }

    void Smartwork()
    {
        _smart_work += 1;
    }
};

int main()
{
    std::vector<BaseStudent*> vec_st;
    vec_st.reserve(1000);
    for (int i = 0; i < 1000; i++)
    {
        vec_st.push_back(new SmartStudent());
    }
    size_t size = vec_st.size();

    std::cout << "vec_st size : " << size << std::endl;

    __int64 tick = GetTickCount64();
    for (int i = 0; i < 10000000; i++)
    {
        int index = i % size;
        dynamic_cast<SmartStudent*>(vec_st[index])->Smartwork();
    }
    std::cout << "cast speed test(dynamic_cast)" << GetTickCount64() - tick << std::endl;

    tick = GetTickCount64();
    for (int i = 0; i < 10000000; i++)
    {
        int index = i % size;
        static_cast<SmartStudent*>(vec_st[index])->Smartwork();
    }
    std::cout << "cast speed test(static_cast)" << GetTickCount64() - tick << std::endl;

    std::vector<BaseStudent*> vec_st2;
    vec_st2.reserve(1000);
    for (int i = 0; i < 1000; i++)
    {
        int random = rand() % 3;
        switch (random)
        {
        case 0:
            vec_st2.push_back(new Student());
            break;
        case 1:
            vec_st2.push_back(new SmartStudent());
            break;
        case 2:
            vec_st2.push_back(new SuperStudent());
            break;
        }
    }

    size_t size2 = vec_st2.size();

    std::cout << "vec_st2 size : " << size2 << std::endl;

    tick = GetTickCount64();
    for (int i = 0; i < 10000000; i++)
    {
        int index = i % size2;
        auto student = dynamic_cast<SmartStudent*>(vec_st2[index]);
        if (student == nullptr)
            continue;
        student->Smartwork();
    }
    std::cout << "TypeCheck version cast speed test(dynamic_cast)" << GetTickCount64() - tick << std::endl;

    tick = GetTickCount64();
    for (int i = 0; i < 10000000; i++)
    {
        int index = i % size2;
        if(vec_st2[index]->TypeCheck(SMART))
            static_cast<SmartStudent*>(vec_st2[index])->Smartwork();
    }
    std::cout << "TypeCheck version cast speed test(static_cast)" << GetTickCount64() - tick << std::endl;

    for (auto iter = vec_st.begin(); iter != vec_st.end();)
    {
        auto ptr = *iter;
        iter = vec_st.erase(iter);
        delete ptr;
    }

    for (auto iter = vec_st2.begin(); iter != vec_st2.end();)
    {
        auto ptr = *iter;
        iter = vec_st2.erase(iter);
        delete ptr;
    }
    std::cout << "nomal : " << nomal << std::endl;
    std::cout << "super  : " << super << std::endl;
    std::cout << "smart  : " << smart << std::endl;

    std::cout << "total    : " << nomal + super + smart << std::endl;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

BaseStudent::~BaseStudent()
{
}
