/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - ENV - ARM9
  File:     env_system.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: env_system.c,v $
  Revision 1.5  2006/12/18 01:31:04  kitase_hirotake
  不必要なデバッグ出力の削除

  Revision 1.4  2006/04/24 00:05:24  okubata_ryoma
  CW2.0の警告レベル強化に伴った変更

  Revision 1.3  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.2  2005/10/12 07:43:10  kitase_hirotake
  add ENV_SetResoruceSetArray()

  Revision 1.1  2005/09/05 01:16:04  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//---- all ENVResource
SDK_WEAK_SYMBOL ENVResource *resourceArray[] = { NULL };

//---- whether if initialized already
static BOOL sInitialized = FALSE;

//---- class name
static char sCurrentClass[ENV_CLASS_NAME_MAX + 1];

//---- top of resource set list
static ENVResourceSetLink *sResourceSetLinkHead = NULL;

//---- resource set list
static ENVResourceSetLink sResourceSetLink[ENV_RESOURCE_SET_MAX];


//---- static functions
static void ENVi_InitResourceSet(ENVResource * resSet);

static ENVResource *ENVi_SearchByMemberNameFromResourceSet(ENVResource * resSet,
                                                           const char *memberName);
static ENVResource *ENVi_SearchByFullNameFromResourceSet(ENVResource * resSet,
                                                         const char *fullName);

static void ENVi_SetNextSetForIter(ENVResourceIter * iter);



static void ENVi_ClearLinkList(void)
{
    //---- clear resource link list
    MI_CpuClear8(&sResourceSetLink[0], sizeof(sResourceSetLink));
}

static ENVResourceSetLink *ENVi_SearchBlankLink(void)
{
    int     n;
    for (n = 0; n < ENV_RESOURCE_SET_MAX; n++)
    {
        if (!sResourceSetLink[n].array)
        {
            return &sResourceSetLink[n];
        }
    }
    return NULL;
}

//================================================================================
//      Initialize and Setting
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENV_Init

  Description:  Initialize ENV library

  Arguments:    resSet : pointer to resource array

  Returns:      None.
 *---------------------------------------------------------------------------*/
//void ENV_Init( ENVResource* resSet )
void ENV_Init(void)
{
    if (!sInitialized)
    {
        sInitialized = TRUE;

        //---- clear resource link list
        ENVi_ClearLinkList();

        //(void)ENV_SetResourceSet( resSet );
        ENV_SetResoruceSetArray(resourceArray);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ENV_AppendResourceSet

  Description:  append resource set

  Arguments:    resSet : pointer to resource array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_AppendResourceSet(ENVResource * resSet)
{
    ENVResourceSetLink *p = sResourceSetLinkHead;
    ENVResourceSetLink *lastp = NULL;
    ENVResourceSetLink *blank = ENVi_SearchBlankLink();

    //---- no space to register new set
    if (!blank)
    {
        return;
    }

    //---- search last pointer
    while (p)
    {
        lastp = p;
        p = p->next;
    }

    blank->array = resSet;
    blank->next = NULL;

    //---- link
    if (lastp)
    {
        lastp->next = blank;
    }
    else
    {
        sResourceSetLinkHead = blank;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ENV_PrependResourceSet

  Description:  prepend resource set

  Arguments:    resSet : pointer to resource array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_PrependResourceSet(ENVResource * resSet)
{
    ENV_InsertResourceSet(NULL, resSet);
}

/*---------------------------------------------------------------------------*
  Name:         ENV_InsertResourceSet

  Description:  insert resource set

  Arguments:    standardResSet : pointer to standard resource array
                resSet         : pointer to resource array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_InsertResourceSet(ENVResource * standardResSet, ENVResource * resSet)
{
    ENVResourceSetLink *blank = ENVi_SearchBlankLink();

    //---- no space to register new set
    if (!blank)
    {
        return;
    }

    blank->array = resSet;

    if (!standardResSet)
    {
        blank->next = sResourceSetLinkHead;
        sResourceSetLinkHead = blank;
    }
    else
    {
        ENVResourceSetLink *p = sResourceSetLinkHead;
        ENVResourceSetLink *lastp = NULL;

        //---- search standard resource set
        while (p)
        {
            if (p->array == standardResSet)
            {
                break;
            }

            p = p->next;
        }

        //---- not found standard resource set
        if (!p)
        {
            return;
        }

        blank->next = p->next;
        p->next = blank;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SetResourceSet

  Description:  set resource set.
                other sets before set will be expired

  Arguments:    resSet : pointer to resource array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_SetResourceSet(ENVResource * resSet)
{
    //---- clear resource link list
    ENVi_ClearLinkList();

    ENV_AppendResourceSet(resSet);
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetResourceSetLinkHead

  Description:  get pointer to first resource link

  Arguments:    None.

  Returns:      pointer to first resource set link
 *---------------------------------------------------------------------------*/
ENVResourceSetLink *ENV_GetResourceSetLinkHead(void)
{
    return sResourceSetLinkHead;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetNextResourceSetLink

  Description:  get next resource link of specified resource link

  Arguments:    link: resource set link.

  Returns:      next resource set link
 *---------------------------------------------------------------------------*/
ENVResourceSetLink *ENV_GetNextResourceSet(ENVResourceSetLink * link)
{
    SDK_ASSERT(link);
    return link->next;
}


//================================================================================
//      Class
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENV_SetClass

  Description:  set current class.
                this value is used to omit the class name
                in getting the resource value.

  Arguments:    className : pointer to resource set

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_SetClass(const char *className)
{
    SDK_WARNING(STD_GetStringLength(className) <= ENV_CLASS_NAME_MAX, "class name is too long");

    //---- remember class name
    (void)STD_CopyLString(sCurrentClass, className, ENV_CLASS_NAME_MAX);
    sCurrentClass[ENV_CLASS_NAME_MAX] = '\0';
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetClass

  Description:  get current class.

  Arguments:    None.

  Returns:      pointer to current class name
 *---------------------------------------------------------------------------*/
char   *ENV_GetClass(void)
{
    return sCurrentClass;
}

//================================================================================
//      compare string
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENVi_CheckIfSameClass

  Description:  check if resource belongs to the class

  Arguments:    name      : resource name
                className : class name

  Returns:      NULL if not belong.
                return pointer to member name if belong.
 *---------------------------------------------------------------------------*/
static char *ENVi_CheckIfSameClass(char *name, char *className)
{
    char   *s1 = className;
    char   *s2 = name;

    while (*s1)
    {
        if (*s1++ != *s2++)
        {
            return NULL;
        }
    }

    //---- check if s2 reaches to the end of class name
    if (*s2 != '.')
    {
        return NULL;
    }

    return s2 + 1;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_CheckIfSameMember

  Description:  check if resource is same to the specified member

  Arguments:    name       : resource name
                memberName : member name

  Returns:      NULL if not same
                return pointer to member name if same
 *---------------------------------------------------------------------------*/
static char *ENVi_CheckIfSameMember(char *name, char *memberName)
{
    char   *s1 = memberName;
    char   *s2 = name;

    //---- skip class
    while (*s2 && *s2 != '.')
    {
        s2++;
    }

    if (!*s2)
    {
        return NULL;
    }
    s2++;

    //----
    while (*s1)
    {
        if (*s1++ != *s2++)
        {
            return NULL;
        }
    }

    //---- check if s2 reaches to the end of member name
    if (*s2 != '.' && *s2 != '\0')
    {
        return NULL;
    }

    return s2 + 1;
}

//================================================================================
//      Search
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENVi_SearchByMemberNameFromResourceSet

  Description:  search a resource by specifying member name from a resource set.
  				use current class name.

  Arguments:    resSet     : pointer to resource set
                memberName : member name

  Returns:      pointer to specified resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
static ENVResource *ENVi_SearchByMemberNameFromResourceSet(ENVResource * resSet,
                                                           const char *memberName)
{
    ENVResource *ir = resSet;

    for (; ir->name; ir++)
    {
        //---- check class name (if differ, return NULL)
        char   *memberPtr = ENVi_CheckIfSameClass(ir->name, sCurrentClass);

        //---- check member name
        if (memberPtr && !STD_CompareString(memberPtr, memberName))
        {
            return ir;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_SearchByMemberName

  Description:  search a resource by specifying member name.
  				use current class name.
				get pointer to resource set.

  Arguments:    memberName : member name
                resSetPtr  : pointer to resource set

  Returns:      pointer to specified resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENVi_SearchByMemberName(const char *memberName, ENVResource ** resSetPtr)
{
    ENVResourceSetLink *link = sResourceSetLinkHead;

    if (link && memberName && *memberName != '\0')
    {
        while (link)
        {
            ENVResource *p = ENVi_SearchByMemberNameFromResourceSet(link->array, memberName);
            if (p)
            {
                if (resSetPtr)
                {
                    *resSetPtr = link->array;
                }
                return p;
            }
            link = link->next;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_SearchByFullNameFromResourceSet

  Description:  search a resource by specifying full name  from a resource set.

  Arguments:    resSet   : pointer to resource set
                fullName : name (full name)

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
static ENVResource *ENVi_SearchByFullNameFromResourceSet(ENVResource * resSet, const char *fullName)
{
    ENVResource *ir = resSet;
#if 0
    OS_Printf("set=%x\n", ir);
#endif
    for (; ir->name; ir++)
    {
#if 0
        OS_Printf("comp %x %x\n", ir->name, fullName);
#endif
        if (!STD_CompareString(ir->name, fullName))
        {
            return ir;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_SearchByFullName

  Description:  search a resource by specifying full name.
				get pointer to resource set.

  Arguments:    fullName : name (full name)
                resSetPtr  : pointer to resource set

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENVi_SearchByFullName(const char *fullName, ENVResource ** resSetPtr)
{
    ENVResourceSetLink *link = sResourceSetLinkHead;

    if (link && fullName && *fullName != '\0')
    {
        while (link)
        {
            ENVResource *p = ENVi_SearchByFullNameFromResourceSet(link->array, fullName);
            if (p)
            {
                if (resSetPtr)
                {
                    *resSetPtr = link->array;
                }
                return p;
            }
            link = link->next;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_Search

  Description:  search a resource by specifying name from all resoruce set.
                if name begins by ".", it will be added to current class.
				get pointer to resource set.

  Arguments:    name (full name or member name of class)
                resSetPtr  : pointer to resource set

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENVi_Search(const char *name, ENVResource ** resSetPtr)
{
    ENVResourceSetLink *link = sResourceSetLinkHead;

    if (link && name && *name != '\0')
    {
        if (*name == '.')
        {
            return ENVi_SearchByMemberName(name + 1, resSetPtr);
        }
        else
        {
            return ENVi_SearchByFullName(name, resSetPtr);
        }
    }

    return NULL;
}

//================================================================================
//      Get value
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENVi_GetPtrAndLength

  Description:  get pointer of specified member name.
                data length is stored to len.

  Arguments:    name : full name or member name of class
                len  : pointer to length

  Returns:      pointer
 *---------------------------------------------------------------------------*/
void   *ENVi_GetPtrAndLength(const char *name, int *len)
{
    ENVResource *resSetPtr;
    ENVResource *p = ENVi_Search(name, &resSetPtr);

    if (!p)
    {
        if (len)
        {
            *len = 0;
        }
        return NULL;
    }

    if (len)
    {
        *len = p->len - ((p->type == ENV_RESTYPE_STRING) ? 1 : 0);
    }

    if (p->type & ENV_RESTYPE_OFFSET_MASK)
    {
        return (void *)((u32)resSetPtr + (u32)(p->ptr));
    }
    else
    {
        return p->ptr;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetType

  Description:  get resource type

  Arguments:    name : full name or member name of class

  Returns:      resource type. ENV_RESTYPE_xxx.
 *---------------------------------------------------------------------------*/
ENVType ENV_GetType(const char *name)
{
    ENVResource *p = ENV_Search(name);
    return (p) ? p->type : ENV_RESTYPE_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetSize

  Description:  get resource size

  Arguments:    name : full name or member name of class

  Returns:      resource size
 *---------------------------------------------------------------------------*/
int ENV_GetSize(const char *name)
{
    ENVResource *p = ENV_Search(name);
    return (p) ? p->len : 0;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetBelogingResourceSet

  Description:  get resource set which res is belong to

  Arguments:    res : pointer to resource

  Returns:      pointer to resoruce set
 *---------------------------------------------------------------------------*/
ENVResource *ENV_GetBelongingResourceSet(ENVResource * res)
{
    ENVResourceSetLink *link = sResourceSetLinkHead;

    while (link)
    {
        ENVResource *resSet = link->array;
        ENVResource *p = resSet;

        while (p->name)
        {
            p++;
        }

        if ((u32)resSet <= (u32)res && (u32)res <= (u32)p)
        {
            return resSet;
        }

        link = link->next;
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_GetU64

  Description:  get u64 value

  Arguments:    name : full name or member name of class
                retVal : pointer to store resource value

  Returns:      u64 value
 *---------------------------------------------------------------------------*/
BOOL ENV_GetU64(const char *name, u64 *retVal)
{
    ENVResource *resSetPtr;
    ENVResource *p = ENVi_Search(name, &resSetPtr);

    if (!p)
    {
        *retVal = 0;
        return FALSE;
    }

    if (p->type & ENV_RESTYPE_OFFSET_MASK)
    {
        *retVal = *(u64 *)((u32)resSetPtr + (u32)(p->ptr));
    }
    else
    {
        //---- check if resource has 64bit value
        SDK_WARNING((p + 1)->name && !STD_CompareString((p + 1)->name, ENVi_DUMMY_FOR_U64),
                    "64bit resource is stored illegally");

        //---- combine 2 resources to u64 value
        *retVal = (u64)(((u64)((p + 1)->ptr) << 32) | (u32)(p->ptr));
    }

    return TRUE;
}

//================================================================================
//    search by using iterator
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         ENV_InitIter

  Description:  Initialize iterator

  Arguments:    iter : pointer to iterator

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_InitIter(ENVResourceIter * iter)
{
    iter->link = sResourceSetLinkHead;
    iter->ptr = iter->link->array;

    iter->count = 0;
}

/*---------------------------------------------------------------------------*
  Name:         ENVi_SetNextSetForIter

  Description:  set next resource set

  Arguments:    iter : pointer to iterator

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ENVi_SetNextSetForIter(ENVResourceIter * iter)
{
    iter->link = iter->link->next;

    if (!iter->link)
    {
        return;
    }

    iter->ptr = iter->link->array;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SearchByClass

  Description:  search resource which has specified class

  Arguments:    iter      : pointer to iterator
                className : class name

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENV_SearchByClass(ENVResourceIter * iter, const char *className)
{
    while (iter->link)
    {
        if (!iter->ptr->name)
        {
            ENVi_SetNextSetForIter(iter);
            if (!iter->link)
            {
                break;
            }
        }

        if (iter->ptr->name)
        {
            char   *memberPtr;

            //---- increase ptr
            ENVResource *p = iter->ptr;
            iter->ptr++;

            //---- check if specified class
            if ((memberPtr = ENVi_CheckIfSameClass(p->name, (char *)className)) != 0)
            {
                iter->supData = (void *)memberPtr;
                iter->count++;
                return p;
            }
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SearchByMember

  Description:  search resource which has specified member

  Arguments:    iter       : pointer to iterator
                memberName : member name

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENV_SearchByMember(ENVResourceIter * iter, const char *memberName)
{
    while (iter->link)
    {
        if (!iter->ptr->name)
        {
            ENVi_SetNextSetForIter(iter);
            if (!iter->link)
            {
                break;
            }
        }

        if (iter->ptr->name)
        {
            //---- increase ptr
            ENVResource *p = iter->ptr;
            iter->ptr++;

            //---- check if specified member
            if (ENVi_CheckIfSameMember(p->name, (char *)memberName))
            {
                iter->supData = (void *)p;
                iter->count++;
                return p;
            }
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SearchByType

  Description:  search resource which is specified type

  Arguments:    iter : pointer to iterator
                type : resource type

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENV_SearchByType(ENVResourceIter * iter, ENVType type)
{
    while (iter->link)
    {
        if (!iter->ptr->name)
        {
            ENVi_SetNextSetForIter(iter);
            if (!iter->link)
            {
                break;
            }
        }

        if (iter->ptr->name)
        {
            //---- increase ptr
            ENVResource *p = iter->ptr;
            iter->ptr++;

            //---- check if specified member
            if (p->type == type)
            {
                iter->count++;
                return p;
            }
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SearchByPartialName

  Description:  search resource whose name contains specified string

  Arguments:    iter        : pointer to iterator
                partialName : string to search in resource name

  Returns:      pointer to resource.
                NULL if not found.
 *---------------------------------------------------------------------------*/
ENVResource *ENV_SearchByPartialName(ENVResourceIter * iter, const char *partialName)
{
    while (iter->link)
    {
        if (!iter->ptr->name)
        {
            ENVi_SetNextSetForIter(iter);
            if (!iter->link)
            {
                break;
            }
        }

        if (iter->ptr->name)
        {
            //---- increase ptr
            ENVResource *p = iter->ptr;
            iter->ptr++;

            //---- check if specified member
            if (STD_SearchString(p->name, partialName) != NULL)
            {
                iter->count++;
                return p;
            }
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         ENV_SetResoruceSetArray

  Description:  set all resoruce set

  Arguments:    array : pointer to resource array array

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ENV_SetResoruceSetArray(ENVResource * array[])
{
    int     n = 0;
    ENVResource *p = array[n];
    while (p != NULL)
    {
        if (n == 0)
        {
            ENV_SetResourceSet(p);
        }
        else
        {
            ENV_AppendResourceSet(p);
        }
        p = array[++n];
    }
}
