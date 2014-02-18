//snookiecatalogv3.cpp

#include "snookiecatalogv3.h"

//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::SearchTree(HTREEITEM ht){
	OutputDebugString("SNOOKIECATALOG::SearchTree\n");
	TREEVIEW * tv;
	SNOOKIENODE * node;
	char namecopy[400];
	char buf[400];
	tv=(this->scc.tv);
	strupr(this->SearchString);
	/*
	Go through the tree, starting at HT and compare it, go to the next one, compare it, etc...
	*/
	//get the name of this one...
	do{
		node=(SNOOKIENODE*)this->scc.tv->GetLParam(ht);//the LParam is the address of it...
		strcpy(namecopy, node->Name);
		strupr(namecopy);

		if(strstr(namecopy, this->SearchString) != 0){
			//make sure it's visible, select it
			sprintf(buf, "Search Found : HTREEITEM %d\n", ht);
			OutputDebugString(buf);
			SendMessage(tv->GetHwnd(), TVM_SELECTITEM, (WPARAM)TVGN_FIRSTVISIBLE , (LPARAM)ht);
			SendMessage(tv->GetHwnd(), TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)ht);
			SetFocus(tv->GetHwnd());
			//return
			OutputDebugString("SNOOKIECATALOG::SearchTree DONE 01\n");
			return SN_OK;
		}
		//no match
		//make ht the next tree node...
		//the priorities here are child, next sibling, parent's next sibling.
		ht=this->scc.tv->GetNext(ht);
	}while(ht != NULL);

	//no match
	MessageBox(0, "No matches...", "Find", MB_OK);
	OutputDebugString("SNOOKIECATALOG::SearchTree DONE 02\n");
	return SN_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::SetSearchString(char * String, int len){
	OutputDebugString("SNOOKIECATALOG::SetSearchString\n");
	ZeroMemory(this->SearchString, MAX_PATH);
	if(len < MAX_PATH){
		CopyMemory(this->SearchString, String, len);
	}
	OutputDebugString("SNOOKIECATALOG::SetSearchString DONE\n");
	return SN_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::Refresh(){
	OutputDebugString("SNOOKIECATALOG::Refresh()\n");
	if(this->Sel != -1){//make sure we have a valid selection
		SNOOKIENODE * node;
		node=this->Nodes[this->Sel];
		char Buf[400];
		char Buf2[400];
		this->scc.Static0->SetText(node->Name);

		BytesToString(Buf2, node->Size);
		sprintf(Buf, "Size: %s (", Buf2);
		BytesToCommaString(Buf2, node->Size);
		strcat(Buf, Buf2);
		strcat(Buf, " bytes)\n");
		this->scc.Static1->SetText(Buf);

		//find out what path it's from now....
		BOOL Done;
		Done=FALSE;
		SNOOKIENODE * Parent;
		Parent=node;
		do{
			//make sure we have valid pointers
			if((Parent != NULL) && (Parent->Parent != NULL)){
				if(Parent->Parent->Id != Parent->Catalog->GetRoot()){
					//the parent is not the ROOT
					Parent=Parent->Parent;
				}else{
					Done=TRUE;
				}
			}else{
				Done=TRUE;
			}
		}while(Done==FALSE);
		sprintf(Buf, "From: %s\n", Parent->Name);

		//now see how many folders/files are in there, if it's a folder
		if(node->FirstChild != NULL){
			__int32 Files;
			__int32 Folders;
			Files=0;
			Folders=0;
			SNOOKIENODE * CurrentNode;
			CurrentNode=node->FirstChild;
			Done=FALSE;
			do{
				//first, adjust Files/Folders to whether this is a folder or a file
				if(CurrentNode->FirstChild==NULL){
					//it's got no first child, so lets assume it's a file
					Files++;
				}else{
					Folders++;
				}
				//now see if we're done or not.
				if(CurrentNode->NextSibling != NULL){
					//it's not invalid, so we're good
					CurrentNode=CurrentNode->NextSibling;
				}else{
					Done=TRUE;
				}
			}while(Done==FALSE);
			sprintf(Buf2, "# of folders: %d\n# of files: %d\n", Folders, Files);
			strcat(Buf, Buf2);
		}

		this->scc.Static2->SetText(Buf);
	}else{
		//if we had no selection
		this->scc.Static0->SetText("");
		this->scc.Static1->SetText("");
		this->scc.Static2->SetText("");

	}

	DebugRefresh();
	OutputDebugString("SNOOKIECATALOG::Refresh() DONE\n");
	return SN_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::Save(){
	OutputDebugString("SNOOKIECATALOG::Save()\n");
	int Iterations;
	if(this->FileName[0] != 0){
		//save a catalog to disk.
		SNOOKIEHEADER Header;
		ZeroMemory(&Header, sizeof(Header));
		CATFILE File;
		File.OpenForWrite(this->FileName);

		Header.NumberOfNodes=this->NumberOfNodes;
		Header.RootNameLength=strlen(this->Nodes[this->Root]->Name)+1;
		File.Write(SNOOKIEDISKID, SNOOKIEDISKIDLENGTH);//write the program ID
		File.Write(&Header, sizeof(Header));//write the header
		File.Write(this->Nodes[this->Root]->Name, Header.RootNameLength);//write the name of the root node.

		//now we have to go through and write all the nodes.
		//so first find the first node we need to write (first child of root), and from there,
		//simply go down the list, recursively.
		SNOOKIEDISKNODE sdn;
		ZeroMemory(&sdn, sizeof(sdn));
		SNOOKIENODE * CurrentNode;
		SNOOKIENODE * NextNode;

		CurrentNode=this->Nodes[this->Root]->FirstChild;
		if(CurrentNode==0){
			//there is nothing catalogued, so stop here.
			File.Close();
			OutputDebugString("SNOOKIECATALOG::Save() DONE 01\n");
			return SN_OK;
		}

		int c;
		for(c=2;c<=this->NumberOfNodes;c++){//NumberOfNodes includes the root, so start at 2
			//CurrentNode is the real node we need to write, and sdn is the disk node...
			//that we need to fill
			if(CurrentNode==0){
				OutputDebugString("Void NODE!!!! Error...\n");
				File.Close();
				OutputDebugString("SNOOKIECATALOG::Save() DONE 02\n");
				return SN_ERROR;
			}
			sdn.HasChildren=(CurrentNode->FirstChild==0) ? FALSE : TRUE;
			sdn.Image=(CurrentNode->FirstChild !=0) ? CATIMAGE_FOLDER : CATIMAGE_FILE;
			sdn.Size=CurrentNode->Size;
			//sdn.MemoLength=CurrentNode->MemoLength+1;
			sdn.NameLength=CurrentNode->NameLength;

			//the real critical part:  the action.
			//really, there's just a priority here.

			//ALSO, here is easiest and fastest to get the nxt node.
			if(CurrentNode->FirstChild != 0){
				//if there's a child, that is the next node.
				sdn.Action=SN_ACTION_CHILD;
				NextNode=CurrentNode->FirstChild;
			}else{
				if(CurrentNode->NextSibling != 0){
					sdn.Action=SN_ACTION_SIBLING;
					NextNode=CurrentNode->NextSibling;
				}else{
					/*
					Now I kinda run into a problem here with the way I'm doing this.
					I need to build a special case with going up to the parent.
					If you need to go up several parents, we need to incorporate that.
					.
					So let's see...we need to make a loop here that keeps testing
					whether or not we have a parent.  If there's no parent, keep looping
					and going up a directory
					*/
					Iterations=1;//the number of parents to move up

					//we need to get the action, whether it's nothing, or a parent.
					//and the number of parents to go up, and then get the nextnode.
					NextNode=CurrentNode->Parent;
					if(NextNode==0){//there's no parent, this is really an error
						OutputDebugString("There's no parent here...#001\n");
						sdn.Action=SN_ACTION_NONE;
					}else{
						//so CurrentNode is the base node, and we need to figure out how many
						//parents to move up before there is a next sibling to the parent.
						//I know, this whole thing is just a way around recursive programming,
						//but I just feel it's not necessary; this is faster adn more organized.

						//we start out this loop with NextNode being the parent.  One iteration.
						while(NextNode->NextSibling == 0){
							//so long as there's no sibling,
							NextNode=NextNode->Parent;//we move up one folder
							if(NextNode==0){//check if it's there
								OutputDebugString("There's no parent here...#002\n");//this will happen when we're done
								sdn.Action=SN_ACTION_NONE;							
								break;
							}
							Iterations++;//adn if it is, we add an iteration
						}
						if(NextNode != 0){//this MIGHT not necessarily be valid, so account for it.
							NextNode=NextNode->NextSibling;
							sdn.Action=SN_ACTION_PARENT;
						}

					}

				}//sibling
			}//child
			File.Write(&sdn, sizeof(sdn));
			File.Write(CurrentNode->Name, sdn.NameLength);
			//File.Write(CurrentNode->Memo, sdn.MemoLength);
			if(sdn.Action==SN_ACTION_PARENT){
				File.Write(&Iterations, sizeof(Iterations));
			}

			CurrentNode=NextNode;
			NextNode=0;
			//now get the next node...
		}

		File.Close();
		this->OnDisk=TRUE;
		OutputDebugString("SNOOKIECATALOG::Save() DONE 03\n");
		return SN_OK;
	}
	OutputDebugString("SNOOKIECATALOG::Save() DONE 04\n");
	return SN_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::Open(char * FileName, SNOOKIEITEM si){
	OutputDebugString("SNOOKIECATALOG::Open()...\n");
	char StatusBarText[256];
	this->scc.StatusBar->GetText(3,StatusBarText);

	char OldStatusText[256];
	this->scc.StatusBar->GetText(1, OldStatusText);

	CATFILE File;
	char ID[SNOOKIEDISKIDLENGTH];
	SNOOKIEHEADER Header;
	SNOOKIEDISKNODE sdn;
	__int64 i,p;
	__int64 Parent;
	__int64 ThisNode;
	RNSTRUCT rn;
	int Iterations;

	this->Clear();

	if(FileName[0] != 0){
		OutputDebugString("Opening...\n");
		File.OpenForRead(FileName);
		//first read the header
		File.Read(ID, SNOOKIEDISKIDLENGTH);//the program ID
		if(strcmp(ID, SNOOKIEDISKID) !=0){
			OutputDebugString("ID not correct...setting up root and exiting open...\n");
			File.Close();
			this->SetupRoot();
			OutputDebugString("SNOOKIECATALOG::Open() DONE 01\n");
			return SN_ERROR;
		}
		File.Read(&Header, sizeof(Header));//the header
		File.Read(rn.Name, Header.RootNameLength);//write the name of the root node.
		rn.Size=0;
		rn.Parent=-1;
		rn.HasChildren=1;
		rn.Image=CATIMAGE_CATALOG;

		//now, we will be registering a series of nodes, so let's
		//allocate for them in one big heap, instead of doing it one-by-one.
		this->scc.StatusBar->SetText(1, "Allocating...");

		this->NAT=new int[(int)Header.NumberOfNodes];
		this->Nodes=new SNOOKIENODE*[(int)Header.NumberOfNodes];
		this->LastIndex=Header.NumberOfNodes;
		this->NextAvailable=0;
		this->NumberOfNodes=0;
		for(i=0;i<Header.NumberOfNodes;i++){
			this->Nodes[i]=0;
			this->NAT[i]=FALSE;
		}
		this->scc.StatusBar->SetText(1, "Opening...");
		this->Root=this->RegisterNewNode(&rn);

		Parent=this->Root;

		for(i=2;i<=Header.NumberOfNodes;i++){
			//and now we GO
			File.Read(&sdn, sizeof(sdn));
			//fill RNSTRUCT with it.
			rn.HasChildren=sdn.HasChildren;
			rn.Image=sdn.Image;
			rn.Parent=(int)Parent;
			rn.Size=sdn.Size;
			File.Read(rn.Name, sdn.NameLength);
			if(sdn.Action==SN_ACTION_PARENT){
				File.Read(&Iterations, sizeof(Iterations));
			}
			if(rn.HasChildren==TRUE) this->scc.StatusBar->SetText(3,rn.Name);
			ThisNode=this->RegisterNewNode(&rn);
			//get the parent.
			switch(sdn.Action){
			case SN_ACTION_PARENT://we need to move up directories for the next one...
				for(p=0;p<Iterations;p++){
					Parent=this->Nodes[Parent]->Parent->Id;
				}
				break;
			case SN_ACTION_NONE://don't make a different.
				break;
			case SN_ACTION_SIBLING://the next node will have the same parent as this one.
				break;
			case SN_ACTION_CHILD://the next node will be the child of this node, so the parent
				//will be THIS node (the one we just registered)
				Parent=ThisNode;
				break;
			}
		}
		File.Close();
		this->scc.StatusBar->SetText(3,StatusBarText);
		this->scc.StatusBar->SetText(1,OldStatusText);
		return SN_OK;
	}
	this->SetupRoot();

	OutputDebugString("SNOOKIECATALOG::Open() DONE 02\n");
	return SN_ERROR;
}


//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIECATALOG::SNOOKIECATALOG(SNCATCONTROLS scc, SVSTRUCT sv){
	OutputDebugString("SNOOKIECATALOG::SNOOKIECATALOG()\n");
	this->scc.tv=scc.tv;
	this->scc.StatusBar=scc.StatusBar;
	this->scc.Static0=scc.Static0;
	this->scc.Static1=scc.Static1;
	this->scc.Static2=scc.Static2;

	this->sv.hInstance=sv.hInstance;

	this->OnDisk=FALSE;
	this->FileName[0]=0;

	this->Sel=-1;
	this->opt.DisplayExtention=TRUE;
	this->opt.ExcludeFiles=FALSE;
	this->opt.OnlyMP3=FALSE;

	this->Root=-1;
	this->NextAvailable=0;
	this->NumberOfNodes=0;
	this->LastIndex=-1;

	this->SetupIcons();

	//set up the root now.
	this->SetupRoot();
	OutputDebugString("SNOOKIECATALOG::SNOOKIECATALOG() DONE\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIECATALOG::~SNOOKIECATALOG(){
	OutputDebugString("SNOOKIECATALOG::~SNOOKIECATALOG()\n");
	OutputDebugString("SNOOKIECATALOG::~SNOOKIECATALOG() DONE\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEITEM SNOOKIECATALOG::GetRoot(){
	return this->Root;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::Clear(){
	//Clears EVERYTHING in the catalog...100 percent destruction...
	//We need to just go down the line and delete them all...I could just this->Remove the
	//root, but that would be slow... this isn't much to write.
	__int64 i;
	OutputDebugString("SNOOKIECATALOG::Clear()\n");

	TreeView_DeleteAllItems(this->scc.tv->GetHwnd());


	for(i=0;i<=this->LastIndex;i++){
		//delete this->Nodes[i] and adjust NAT accordingly.
		if(this->NAT[i]==TRUE){
			//we only need to do something if this is TRUE
			if(this->Nodes[i]->Name != 0){
				delete this->Nodes[i]->Name;
				this->Nodes[i]->Name=0;
			}
			if(this->Nodes[i]->Memo != 0){
				delete this->Nodes[i]->Memo;
				this->Nodes[i]->Memo=0;
			}
			//get it off the tree.
//			TreeView_DeleteItem(this->scc.tv->GetHwnd(), this->Nodes[i]->hTreeItem);

			delete this->Nodes[i];
			this->Nodes[i]=0;
		}
	}
	if(this->NAT != 0){
		delete this->NAT;
		this->NAT=0;
	}
	if(this->Nodes != 0){
		delete this->Nodes;
		this->Nodes=0;
	}
	this->LastIndex=-1;
	this->NumberOfNodes=0;
	this->Root=-1;
	this->NextAvailable=0;
	this->Sel=-1;

	OutputDebugString("SNOOKIECATALOG::Clear() DONE\n");
	return SN_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::SetupRoot(){//the root must be -1 for this to happen
	OutputDebugString("SNOOKIECATALOG::SetupRoot()\n");
	if(this->Root==-1){
		RNSTRUCT rn;
		ZeroMemory(&rn, sizeof(rn));
		strcpy(rn.Name, "Empty Catalog");
		rn.Parent=-1;
		rn.HasChildren=1;
		rn.Image=CATIMAGE_CATALOG;

		this->Root=this->RegisterNewNode(&rn);
		OutputDebugString("SNOOKIECATALOG::SetupRoot() DONE 01\n");
		return SN_OK;
	}
	OutputDebugString("SNOOKIECATALOG::SetupRoot() DONE 02\n");
	return SN_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::SaveAs(char * FileName){
	OutputDebugString("SNOOKIECATALOG::SaveAs()\n");
	//simply copy the filename to this filename, and save it.
	strcpy(this->FileName, FileName);
	this->Save();
	OutputDebugString("SNOOKIECATALOG::SaveAs() DONE\n");
	return SN_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//will setup 1 icon for a particular SNOOKIECATALOG
int SetupIcon(SNOOKIECATALOG* sc, int IdSel, int IdNorm, int CatId){
	HBITMAP norm, sel;
	norm=LoadBitmap(sc->GetInstance(), MAKEINTRESOURCE(IdNorm));
	if(norm==0) OutputDebugString("NOPE\n");
	sel=LoadBitmap(sc->GetInstance(), MAKEINTRESOURCE(IdSel));
	if(sel==0) OutputDebugString("NOPE\n");
	(sc->GetTree())->AddImage(&sc->ImageId[CatId], norm, sel);

	DeleteObject(norm);
	DeleteObject(sel);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////
TREEVIEW* SNOOKIECATALOG::GetTree(){
	return this->scc.tv;
}
///////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE SNOOKIECATALOG::GetInstance(){
	return this->sv.hInstance;
}
///////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::SetupIcons(){
	//now read in all the icons.
	SetupIcon(this, IDB_FOLDER_SEL, IDB_FOLDER_NORM, CATIMAGE_FOLDER);
	SetupIcon(this, IDB_FILE_NORM, IDB_FILE_NORM, CATIMAGE_FILE);
	SetupIcon(this, IDB_CATALOG_NORM, IDB_CATALOG_NORM, CATIMAGE_CATALOG);
	return SN_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////
SNOOKIEITEM SNOOKIECATALOG::RegisterNewNode(RNSTRUCT *rn){
	char Buf[300];
	Buf[0]=0;
	//here's where the real mechanics kick in.
	//this function's purpose is to enter a new node into teh catalog.
	//it's given the name of the node, the parent, and some other info. about the node.
	//we have to dynamically allocate for the node

	//create a new node, first.
	SNOOKIENODE *NewNode=new SNOOKIENODE;
	if(NewNode==0) return SN_INSUFFICIENTMEMORY;

	//allocate for the name for both the NAME and ORIGINAL NAME
	NewNode->NameLength=strlen(rn->Name)+1;

	NewNode->Name=new char[NewNode->NameLength+1];//plus one to accomodate for a 0 term.
	if(NewNode->Name==0) return SN_INSUFFICIENTMEMORY;

	//now that it's allocated, copy the stuff...
	strcpy(NewNode->Name, rn->Name);

	NewNode->Size=rn->Size;
	NewNode->MemoLength=0;
	NewNode->Memo=0;
//	NewNode->NumberOfFiles=rn->NumberOfFiles;
//	NewNode->NumberOfFolders=rn->NumberOfFolders;
//	NewNode->NumberOfImmediateFiles=rn->NumberOfImmediateFiles;
//	NewNode->NumberOfImmediateFolders=rn->NumberOfImmediateFolders;
	//get the parent node for it.
	BOOL IsRoot;//is this the root directory?

	if(rn->Parent != -1){
		NewNode->Parent=this->Nodes[rn->Parent];
		IsRoot=FALSE;
	}else{
		IsRoot=TRUE;
		NewNode->Parent=0;//parent is ITSELF, if it's the root
	}
	//if the parent node is -1, then it's the root

	NewNode->FirstChild=0;//NULL pointers
	NewNode->NextSibling=0;
	NewNode->LastChild=0;

	if(IsRoot == FALSE){//if it's not the root...if it's the root, then we won't have to worry about anything with parents or siblings.
		NewNode->PreviousSibling=NewNode->Parent->LastChild;
		//we have to setup the links now...
		SNOOKIENODE *NewParent;//a pointer to this node's parent.
		SNOOKIENODE *OldLastSibling;//a pointer to this node's parent's previous LAST child (it won't be the last child anymore, now that we're adding another.)
		NewParent=NewNode->Parent;
		OldLastSibling=NewNode->Parent->LastChild;
		if(NewParent->FirstChild==0){//if that pointer is null
			//if there weren't any children for that parent,
			//then this will be both the FIRST and LAST children
			NewParent->FirstChild=NewNode;
		}
		NewParent->LastChild=NewNode;
	
		//now setup the siblings
		if(OldLastSibling != 0){//make sure there WAS a last sibling
			//and give it a NEXT sibling
			OldLastSibling->NextSibling=NewNode;
		}
	}else{
		//if it IS the root, we need to set these to 0
		NewNode->PreviousSibling=0;
	}


	NewNode->Catalog=this;

	int id;
	//we now have a new node...let's allocate memory on the linked list if we need to
	if(this->NextAvailable==this->LastIndex+1){
		//the next available node is 1 past the last one, so let's allocate memory.
		//ugh.
		SNOOKIENODE ** NewList=new SNOOKIENODE*[(int)this->LastIndex+2];
		if(NewList==0) return SN_INSUFFICIENTMEMORY;
		//copy the values from the old list, but only if there is memory allocated
		if(this->NumberOfNodes != 0){
			CopyMemory(NewList, this->Nodes, (int)(sizeof(SNOOKIENODE*)*(this->LastIndex+1)));
			delete this->Nodes;
		}
		this->Nodes=NewList;

		//allocate on the NAT
		BOOL * NewNat = new BOOL[(int)this->LastIndex+2];
		if(NewNat==0) return SN_INSUFFICIENTMEMORY;
		if(this->NumberOfNodes != 0){
			CopyMemory(NewNat, this->NAT, (int)(sizeof(BOOL)*(this->LastIndex+1)));
			delete this->NAT;
		}
		this->NAT=NewNat;
		this->LastIndex++;
	}
	id=(int)this->NextAvailable;//the index of the node we'll write to.
	NewNode->Id=id;
	//write the node
	this->Nodes[id]=NewNode;
	this->NumberOfNodes++;


	//OK, now it's written; we need to truly REGISTER it by updating the NAT - Node 
	//allocation table.
	this->NAT[id]=TRUE;

	//now adjust the next available and stuff...
	//if the NextAvailable == LastIndex+1 then we just need to increase NextAvailable.
	if(this->NextAvailable == (this->LastIndex+1)){
		this->NextAvailable++;
	}else{
		//we need to find the next available node.
		//start from the NextAvailable and go until we hit a FALSE
		BOOL Done=FALSE;
		do{
			this->NextAvailable++;
			//if we've gone past the end of the list, then we're done; there aren't any free
			//and NextAvailable will point to one past the end
			if(this->NextAvailable == (this->LastIndex+1)){
				Done=TRUE;
			}else{
				if(this->NAT[this->NextAvailable] == FALSE){
					//this one's OK
					Done=TRUE;
				}
			}
		}while(Done==FALSE);
	}
	//put it on the tree
	//get the parent node, first.
	HTREEITEM Parent=0;
	if(rn->Parent != -1){
		Parent=this->Nodes[rn->Parent]->hTreeItem;
	}

	this->scc.tv->AddNode(&this->Nodes[id]->hTreeItem, Parent, rn->Name,
		rn->HasChildren,(long)this->Nodes[id], rn->Image);

	return id;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//this removes a node and all child nodes, deallocating memory for them.
int SNOOKIECATALOG::Remove(SNOOKIEITEM si){
	if(si!=0){
		BOOL done=FALSE;
		if(this->Nodes[si]->FirstChild != 0){
			//it has children, so delete them first.

			//this gets complicated because FirstChild will keep changing, until it's 0
			SNOOKIEITEM iCurrentChild=this->Nodes[si]->FirstChild->Id;
			do{
				this->Remove(iCurrentChild);
				if(iCurrentChild=this->Nodes[si]->FirstChild ==0){
					//we're done now
					break;
				}
				iCurrentChild=this->Nodes[si]->FirstChild->Id;
			}while(done==FALSE);
		}
		//now we're ready to delete this node and fix all pointers to it (ugh.)
		//and deallocate memory for the memo, name, original name, and object itself.

		//let's simplify what we call the node to delete
		SNOOKIENODE * ThisNode=this->Nodes[si];
		/*
		We need to fix links in the list now...
		Now here's a list of targeted pointers:
		ThisNode->FirstChild; 
		ThisNode->LastChild;
		ThisNode->NextSibling;
		ThisNode->PreviousSibling;
		ThisNode->Parent;

		The first child will belong to the parent of this node, but only if they're equal.
		If it's equal, then the first child will become this node's next sibling.

		The LastChild will be the same thing, except the paren'ts LastChild will become
		this nodes' previous sibling.

		NextSibling - The PreviousSibling will point to this node - make it point to NextSibling

		PreviousSibling - NextSibling's PrevSibilin will need to point to PreviousSibling

		Parent will remain unchanged.
		*/
		//So what we need to fix will be:
		//PARENT->FIRSTCHILD
		//PARENT->LASTCHILD
		//NEXTSIBLING->PREVIOUSSIBLING
		//PREVIOUSSIBLING->NEXTSIBLING

		char Buf[200];
		sprintf(Buf, "Deleting...%s\n", ThisNode->Name);
		OutputDebugString(Buf);

		//now fix all associations first.
		//PARENT->FIRSTCHILD
		if(ThisNode->Parent->FirstChild==ThisNode){
			//this is the first child, so fix some stuff.
			if(ThisNode->NextSibling != 0){
				//There's a next, so that will be the new first child of the parent.
				ThisNode->Parent->FirstChild=ThisNode->NextSibling;
			}else{
				//looks like ThisNode was an only child; parent now has no children.
				ThisNode->Parent->FirstChild=0;
			}
		}
		//PARENT->FIRSTCHILD
		if(ThisNode->Parent->LastChild==ThisNode){
			//this is the last child; no next child
			if(ThisNode->PreviousSibling != 0){
				//we have a previous; use it
				ThisNode->Parent->LastChild=ThisNode->PreviousSibling;
			}else{
				//a lonely only-child node will be put out of its misery, I guess...
				ThisNode->Parent->LastChild=0;
			}
		}
		//NEXTSIBLING->PREVIOUSSIBLING
		if(ThisNode->NextSibling != 0){
			//there'a a next sibling; fix the previous sibling association
			if(ThisNode->PreviousSibling != 0){
				//we have a previous for it
				ThisNode->NextSibling->PreviousSibling=ThisNode->PreviousSibling;
			}else{
				//there wasn't a previous; it's left previous-less
				ThisNode->NextSibling->PreviousSibling=0;
			}
		}
		//PREVIOUSSIBLING->NEXTSIBLING
		if(ThisNode->PreviousSibling != 0){
			//there's a previous sibling we need to fix
			if(ThisNode->NextSibling != 0){
				//and there's a next sibling for him
				ThisNode->PreviousSibling->NextSibling=ThisNode->NextSibling;
			}else{
				ThisNode->PreviousSibling->NextSibling=0;
			}
		}
		//now nothing should point to this node; we can delete it.
		this->NAT[ThisNode->Id]=FALSE;//make it not available.

		if(ThisNode->Memo != 0){
			delete ThisNode->Memo;
			ThisNode->Memo=0;
		}
		if(ThisNode->Name != 0){
			delete ThisNode->Name;
			ThisNode->Name=0;
		}
	
		//get it off the tree.
		TreeView_DeleteItem(this->scc.tv->GetHwnd(), ThisNode->hTreeItem);

		delete ThisNode;
		ThisNode=0;

		//and repair the next available node thingy
		if(this->NextAvailable > si){
			this->NextAvailable=si;
		}

		this->NumberOfNodes--;

	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
int SNOOKIECATALOG::nmSelect(LPNMTREEVIEW lParam){
	SNOOKIENODE * Sel;
	Sel=(SNOOKIENODE*)lParam->itemNew.lParam;
	this->Sel=Sel->Id;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//we're given the index to a snookieitem and a file name.. that filename may be either
//a folder or a file - this will recursively fill a node with child nodes, representing
//the disk structure.
SNOOKIEITEM SNOOKIECATALOG::FillFromDisk(char * FileName, SNOOKIEITEM si, SN_FFD * ffd){
	char Buf[300];
	Buf[0]=0;

	DWORD rvalue;//return value for WinAPI functions
	BOOL IsFolder=FALSE;

	//so we have to figure out if this is a file or a folder.
	_WIN32_FILE_ATTRIBUTE_DATA FileInformation;
	GetFileAttributesEx(FileName, GetFileExInfoStandard, &FileInformation);

	if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
		IsFolder=TRUE;
//		ffd->NumberOfFolders++;
	}else{
		//it's a file
		IsFolder=FALSE;
//		ffd->NumberOfFiles++;
		ffd->Size+=FileInformation.nFileSizeLow+(FileInformation.nFileSizeHigh*MAXDWORD);
	}


	char Path[MAX_PATH];//for "C:\temp\Autoexec.bat", this is "C:\temp"
	char File[MAX_PATH];//for "C:\temp\autoexec.bat", this is "Autoexec.bat"
	rvalue=GetFileTitle(FileName, File, MAX_PATH);
	//if that failed, it's because it's the root path.  a folder, obviously
	if(rvalue != 0){
		strcpy(File, FileName);
	}
	Path[0]=0;

	RNSTRUCT rn;
//	rn.NumberOfFiles=ffd->NumberOfFiles;
//	rn.NumberOfFolders=ffd->NumberOfFolders;
	//these will be changed later
//	rn.NumberOfImmediateFiles=0;
//	rn.NumberOfImmediateFolders=0;

	rn.Size=(FileInformation.nFileSizeHigh * MAXDWORD) + FileInformation.nFileSizeLow;
	rn.HasChildren=IsFolder;//if it's a folder, it will have children
	rn.Image=(IsFolder==TRUE) ? CATIMAGE_FOLDER : CATIMAGE_FILE;
	strcpy(rn.Name,File);
	//if the name ends in a "\" then it's a drive, and we need to get the name of the volume
	if(File[strlen(File)-1]=='\\'){
		GetVolumeInformation(File, rn.Name, 300, 0, 0, 0, 0, 0);
	}
	rn.Parent=si;
	SNOOKIEITEM ThisNode;
	ThisNode=this->RegisterNewNode(&rn);
	if(ThisNode==SN_INSUFFICIENTMEMORY){
		return SN_INSUFFICIENTMEMORY;
	}

	//if it's a folder, then fill under it
	if(IsFolder==TRUE){
		//save the current path
		char OldPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, OldPath);

		//first, get in that directory.	
		if(SetCurrentDirectory(FileName)==0){
			//there was an error
			rvalue=GetLastError();
			if(rvalue==ERROR_NOT_READY){
				//drive not ready
				MessageBox(0, "The drive isn't ready", "Snookie", MB_OK | MB_ICONEXCLAMATION);
			}
			return 0;
		}
		//get the files now, in a loop.
		_WIN32_FIND_DATAA FindData;//info about the find.
		HANDLE hFindFile;//handle to the find
		hFindFile=FindFirstFile("*.*", &FindData);

		//we need to make sure we pass the full path to this function, so...
		char ** FullPath=0;


//		this->Nodes[ThisNode]->NumberOfImmediateFiles=0;
//		this->Nodes[ThisNode]->NumberOfImmediateFolders=0;
		SN_FFD x;
		ZeroMemory(&x, sizeof(SN_FFD));
		do{
			//make sure we don't do the "same directory" or "parent directory" directories
			if(strcmp(FindData.cFileName,".")!=0 && strcmp(FindData.cFileName,"..")!=0){
				//get the full path name
				GetFullPathName(FindData.cFileName, MAX_PATH, Path, FullPath);

				if(this->FillFromDisk(Path, ThisNode, &x) == SN_INSUFFICIENTMEMORY){
					return SN_INSUFFICIENTMEMORY;
				}
			}
		}while(FindNextFile(hFindFile, &FindData) != 0);
//		this->Nodes[ThisNode]->NumberOfImmediateFiles++;
//		this->Nodes[ThisNode]->NumberOfImmediateFolders++;

		FindClose(hFindFile);
		SetCurrentDirectory(OldPath);
		this->Nodes[ThisNode]->Size=x.Size;
		ffd->Size+=x.Size;
//		this->Nodes[ThisNode]->NumberOfFiles=(int)ffd->NumberOfFiles;
//		this->Nodes[ThisNode]->NumberOfFolders=(int)ffd->NumberOfFolders;
	}

	return si;
}


/////////////////////////////////////////////////////////////////////////////////////////
char* BytesToString(char* buf, LONGLONG Bytes){
	//converts number of bytes into a string expression
	//it makes it precise to two decimal places.

	if(Bytes<1024){
		//bytes
		sprintf(buf, "%d bytes", Bytes);
	}else{
		if(Bytes<(1024*1024)){
			//kb
			sprintf(buf, "%.2f kb", (long double)Bytes/(1024));
		}else{
			if(Bytes<(1024*1024*1024)){
				//mb
				sprintf(buf, "%.2f mb", (long double)Bytes/(1024*1024));
			}else{
				//gb
				sprintf(buf, "%.2f gb", (long double)Bytes/(1024*1024*1024));
			}
		}
	}
	return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////
char* BytesToCommaString(char* buf, LONGLONG Bytes){
	//converts a number to a better looking number; for example:
	//input value: 12407243
	//output:    12,407,243
	//cool, eh?
	char temp[50];
	ZeroMemory(temp, sizeof(temp));
	sprintf(temp, "%I64d\0",Bytes);
	//now that we have it in string form, go through and put commas in.
	//we'll do it kinda funky - we'll start at the end of the temp string
	//and put a comma in every 3 places.  When we're finished; reverse the string
	//and POOF- done.
	int ptemp;
	int pbuf;
	ptemp=0;
	pbuf=0;

	do{
		buf[pbuf]=temp[strlen(temp)-ptemp-1];
		if(((float)(ptemp+1)/3) == (int)((float)(ptemp+1)/3)){
			pbuf++;
			buf[pbuf]=',';
		}
		pbuf++;
		ptemp++;
	}while(temp[ptemp] != (char)0);
	if(buf[pbuf-1]==',') buf[pbuf-1]=0;
	buf[pbuf]=0;
	strrev(buf);
	return buf;
}

/*////////////////////////////////////////////////////////////////////////////////////////
This function finds the next node in the collection, whether it be first child, parent's next,
or next sibling.  Relationship will be returned one of the following:
GNN_NEXTSIBLING
GNN_FIRSTCHILD
GNN_PARENT

If it's GNN_PARENT, then Iterations will be returned the number of parents it had to go up
//*///////////////////////////////////////////////////////////////////////////////////////
SNOOKIENODE * GetNextNode(SNOOKIENODE * Node, int * Relationship, int * Iterations){
	SNOOKIENODE * NextNode;

	if(Node->FirstChild != 0){
		NextNode=Node->FirstChild;
		*Relationship=GNN_FIRSTCHILD;
	}else{
		if(Node->NextSibling != 0){
			NextNode=Node->NextSibling;
			*Relationship=GNN_NEXTSIBLING;
		}else{
			NextNode=Node->Parent;
			*Relationship=GNN_PARENT;
			if(NextNode==0){
				OutputDebugString("There's no parent here...#001\n");
			}else{
				*Iterations=1;
				while(NextNode->NextSibling == 0){
					NextNode=NextNode->Parent;
					if(NextNode==0){
						OutputDebugString("There's no parent here...#002\n");//this will happen when we're done
						break;
					}
					*Iterations++;
				}
				if(NextNode != 0){
					NextNode=NextNode->NextSibling;
				}
			}
		}
	}
	return NextNode;
}