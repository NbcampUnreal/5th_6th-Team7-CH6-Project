#include "GAS/Monster/MonsterBase.h"
#include "GAS/Monster/MonsterAttributeSet.h"

AMonsterBase::AMonsterBase()
{
	MonsterAttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MonsterAttributeSet"));
}
