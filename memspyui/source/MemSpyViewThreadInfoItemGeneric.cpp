/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "MemSpyViewThreadInfoItemGeneric.h"

// System includes
#include <AknWaitDialog.h>
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewThreadInfoItemList.h"

// Constants
const TInt KMemSpyConstructionCheckerTimerPeriod = 500000; // 1/2 second



CMemSpyViewThreadInfoItemGeneric::CMemSpyViewThreadInfoItemGeneric( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aInfoContainer, TMemSpyThreadInfoItemType aType )
:   CMemSpyViewBase( aEngine, aObserver ), iContainer( aInfoContainer )
    {
    iContainer.Thread().Process().Open();
    iContainer.Thread().Open();
    iContainer.Open();
    //
    iInfoItem = &iContainer.Item( aType );
    __ASSERT_ALWAYS( iInfoItem != NULL, User::Invariant() );
    iInfoItem->Open();
    }


CMemSpyViewThreadInfoItemGeneric::~CMemSpyViewThreadInfoItemGeneric()
    {
    if  ( iInfoItem )
        {
        iInfoItem->Close();
        }
    //
    DestroyWaitNote();
    //
    iContainer.Close();
    iContainer.Thread().Close();
    iContainer.Thread().Process().Close();
    }


void CMemSpyViewThreadInfoItemGeneric::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    const TPtrC pTitle( iInfoItem->Name().Mid( 1 ) );
    SetTitleL( pTitle );
    //
    if  ( iInfoItem->IsReady() == EFalse )
        {
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyViewThreadInfoItemGeneric::ConstructL() - show wait note - item not ready, iType: %d", iInfoItem->Type() );
#endif

        // Wait for engine data to be made ready... 
        ShowWaitNoteL();
        }

    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


CMemSpyProcess& CMemSpyViewThreadInfoItemGeneric::Process() const
    {
    return iContainer.Thread().Process();
    }


CMemSpyThread& CMemSpyViewThreadInfoItemGeneric::Thread() const
    {
    return iContainer.Thread();
    }


CMemSpyThreadInfoContainer& CMemSpyViewThreadInfoItemGeneric::Container() const
    {
    return iContainer;
    }


CMemSpyThreadInfoItemBase& CMemSpyViewThreadInfoItemGeneric::InfoItem() const
    {
    __ASSERT_ALWAYS( iInfoItem != NULL, User::Invariant() );
    return *iInfoItem;
    }


void CMemSpyViewThreadInfoItemGeneric::RefreshL()
    {
    iInfoItem->RebuildL();    
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewThreadInfoItemGeneric::ViewType() const
    {
    TMemSpyViewType type = EMemSpyViewTypeNone;
    //
    switch( iInfoItem->Type() )
        {
    case EMemSpyThreadInfoItemTypeHeap:
        type = EMemSpyViewTypeThreadInfoItemHeap;
        break;
    case EMemSpyThreadInfoItemTypeStack:
        type = EMemSpyViewTypeThreadInfoItemStack;
        break;
    case EMemSpyThreadInfoItemTypeChunk:
        type = EMemSpyViewTypeThreadInfoItemChunk;
        break;
    case EMemSpyThreadInfoItemTypeCodeSeg:
        type = EMemSpyViewTypeThreadInfoItemCodeSeg;
        break;
    case EMemSpyThreadInfoItemTypeServer:
        type = EMemSpyViewTypeThreadInfoItemServer;
        break;
    case EMemSpyThreadInfoItemTypeSession:
        type = EMemSpyViewTypeThreadInfoItemSession;
        break;
    case EMemSpyThreadInfoItemTypeSemaphore:
        type = EMemSpyViewTypeThreadInfoItemSemaphore;
        break;
    case EMemSpyThreadInfoItemTypeMutex:
        type = EMemSpyViewTypeThreadInfoItemMutex;
        break;
    case EMemSpyThreadInfoItemTypeTimer:
        type = EMemSpyViewTypeThreadInfoItemTimer;
        break;
    case EMemSpyThreadInfoItemTypeLDD:
        type = EMemSpyViewTypeThreadInfoItemLDD;
        break;
    case EMemSpyThreadInfoItemTypePDD:
        type = EMemSpyViewTypeThreadInfoItemPDD;
        break;
    case EMemSpyThreadInfoItemTypeLogicalChannel:
        type = EMemSpyViewTypeThreadInfoItemLogicalChannel;
        break;
    case EMemSpyThreadInfoItemTypeChangeNotifier:
        type = EMemSpyViewTypeThreadInfoItemChangeNotifier;
        break;
    case EMemSpyThreadInfoItemTypeUndertaker:
        type = EMemSpyViewTypeThreadInfoItemUndertaker;
        break;
    case EMemSpyThreadInfoItemTypeMessageQueue:
        type = EMemSpyViewTypeThreadInfoItemMessageQueue;
        break;
    case EMemSpyThreadInfoItemTypeConditionalVariable:
        type = EMemSpyViewTypeThreadInfoItemConditionalVariable;
        break;
    case EMemSpyThreadInfoItemTypeOpenFiles:
        type = EMemSpyViewTypeThreadInfoItemOpenFiles;
        break;
    case EMemSpyThreadInfoItemTypeActiveObject:
        type = EMemSpyViewTypeThreadInfoItemActiveObject;
        break;
    case EMemSpyThreadInfoItemTypeGeneralInfo:
        type = EMemSpyViewTypeThreadInfoItemGeneralInfo;
        break;
    case EMemSpyThreadInfoItemTypeOtherThreads:
        type = EMemSpyViewTypeThreadInfoItemOtherThreads;
        break;
    case EMemSpyThreadInfoItemTypeOtherProcesses:
        type = EMemSpyViewTypeThreadInfoItemOtherProcesses;
        break;
    case EMemSpyThreadInfoItemTypeOwnedThreadHandles:
        type = EMemSpyViewTypeThreadInfoItemOwnedThreadHandles;
        break;
    case EMemSpyThreadInfoItemTypeOwnedProcessHandles:
        type = EMemSpyViewTypeThreadInfoItemOwnedProcessHandles;
        break;

    default:
        __ASSERT_DEBUG( EFalse, User::Invariant() );
        break;
        }
    //
    return type;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemGeneric::PrepareParentViewL()
    {
    CMemSpyViewThreadInfoItemList* parent = new(ELeave) CMemSpyViewThreadInfoItemList( iEngine, iObserver, Thread() );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), iInfoItem );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewThreadInfoItemGeneric::PrepareChildViewL()
    {
    __ASSERT_ALWAYS( iInfoItem != NULL, User::Invariant() );
    CMemSpyViewBase* child = NULL;
    //
    return child;
    }


TBool CMemSpyViewThreadInfoItemGeneric::HandleCommandL( TInt aCommand )
    {
    TBool handled = CMemSpyViewBase::HandleCommandL( aCommand );
    return handled;
    }


void CMemSpyViewThreadInfoItemGeneric::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iInfoItem );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewThreadInfoItemGeneric::HandleListBoxItemActionedL( TInt /*aIndex*/ )
    {
    // Notify observer about an item being 'fired'
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewThreadInfoItemGeneric::HandleListBoxItemSelectedL( TInt /*aIndex*/ )
    {
    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }



void CMemSpyViewThreadInfoItemGeneric::ShowWaitNoteL()
    {
    // Ugly, but I'm not adding an observer mechanism just for this wait dialog.
    __ASSERT_ALWAYS( iWaitConstructionChecker == NULL, User::Invariant() );
    iWaitConstructionChecker = CPeriodic::NewL( CActive::EPriorityLow );
    iWaitConstructionChecker->Start( KMemSpyConstructionCheckerTimerPeriod, 
                                     KMemSpyConstructionCheckerTimerPeriod, 
                                     TCallBack( CheckForItemConstructionComplete, this ) );

    if  ( !iWaitNote )
        {
        iWaitNote = new ( ELeave ) CAknWaitDialog( reinterpret_cast<CEikDialog**> ( &iWaitNote ), ETrue );
        iWaitNote->ExecuteDlgLD( CAknNoteDialog::ENoTone, R_MEMSPY_PREPARING_INFO_ITEM_CONTAINER_WAIT_NOTE );
        }
    }


void CMemSpyViewThreadInfoItemGeneric::DestroyWaitNote()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreadInfoItemGeneric::DestroyWaitNote() - START" );
#endif
	//
    delete iWaitConstructionChecker;
    iWaitConstructionChecker = NULL;
    //
    if  ( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->ProcessFinishedL() ); // deletes the dialog
        iWaitNote = NULL;
        }
	//
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreadInfoItemGeneric::DestroyWaitNote() - END" );
#endif
    }


TInt CMemSpyViewThreadInfoItemGeneric::CheckForItemConstructionComplete( TAny* aSelf )
    {
    CMemSpyViewThreadInfoItemGeneric& self = *reinterpret_cast< CMemSpyViewThreadInfoItemGeneric* >( aSelf );
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreadInfoItemGeneric::CheckForItemConstructionComplete() - ready status: %d, iType: %d", self.iInfoItem->IsReady(), self.iInfoItem->Type() );
#endif
	//
    TBool callAgain = ETrue;
    if  ( self.iInfoItem->IsReady() )
        {
        self.DestroyWaitNote();
        callAgain = EFalse;
        }
    //
    return callAgain;
    }





