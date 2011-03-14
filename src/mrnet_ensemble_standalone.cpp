#include "foo_mrmr.h"

void remove_equiv_subtrees(tree<int>& res, tree<double>& res_mean){
	tree<int>::pre_order_iterator it,it_tmp,it_main, it_back;
	tree<double>::pre_order_iterator it_mean,it_tmp_mean,it_main_mean, it_back_mean;
	int *vec1, *vec2, *vec2_sorted, *vec_main,*vec_main_sorted,length_vec;
	bool equiv;
	double  *vec_tmp_mean, *vec_main_mean;
	
	it=res.begin();it_mean=res_mean.begin();
	it++;it_mean++;
	
	length_vec=res.depth(res.begin_leaf(it));
	
	vec1=new int [length_vec]; vec2=new int [length_vec]; vec_main=new int [length_vec];vec_main_sorted=new int [length_vec];vec2_sorted=new int [length_vec];
	vec_main_mean=new double [length_vec]; vec_tmp_mean=new double[length_vec];
	
	for(unsigned int j=0;j< length_vec;++j){
		vec1[j]=0;
	}
	
	int depth_old=0;
	it_main=it;
	it_main_mean=it_mean;
	bool new_main=false;
	while(res.is_valid(it) && res.is_valid(it_main)){
		vec1[res.depth(it)-1]=*it;
		
		vec_main_mean[res.depth(it)-1]=*it_mean;
		it_back=it;it_back_mean=it_mean;
		
		it++;it_mean++;
		if( depth_old == length_vec ){
			it_main=it;it_main_mean=it_mean;
			
			for(unsigned int j=0;j< length_vec;++j){
				vec_main[j]=vec1[j];
				vec_main_sorted[j]=vec_main[j];
			}
			if(res.depth(it_main)>0){
				for(unsigned int j=0;j< (res.depth(it_main)-1);++j){
					vec2[j]=vec_main[j];
					vec_tmp_mean[j]=vec_main_mean[j];
				}
			}
			it_tmp=it_main;it_tmp_mean=it_main_mean;
			it_main++;it_main_mean++;
			sort(vec_main_sorted,vec_main_sorted+length_vec);
			new_main=false;
			while(res.is_valid(it_tmp) && new_main==false){
				vec2[res.depth(it_tmp)-1]=*it_tmp;
				vec_tmp_mean[res.depth(it_tmp)-1]=*it_tmp_mean;
				if(res.depth(it_tmp)==length_vec){
					for(unsigned int j=0;j< length_vec;++j){
						vec2_sorted[j]=vec2[j];
					}
					sort(vec2_sorted,vec2_sorted+length_vec);
					equiv=true;
					for(unsigned int j=0;j< length_vec && equiv;++j){
						if(vec_main_sorted[j]!=vec2_sorted[j]){
							equiv=false;
						}
					}
					if(equiv){
						if(vec_tmp_mean[length_vec-1]<vec_main_mean[length_vec-1]){
							it_back=it_tmp;it_back_mean=it_tmp_mean;
							it_back++;it_back_mean++;
							res.erase(it_tmp);	res_mean.erase(it_tmp_mean);
							it_tmp=it_back;it_tmp_mean=it_back_mean;
						}else {
							if(*it_main_mean==vec_main_mean[length_vec-1]){
								it_back=it_main;it_back_mean=it_main_mean;
								it_back++;
								it_back_mean++;
								res.erase(it_main);	res_mean.erase(it_main_mean);
								it_main=it_back; it_main_mean=it_back_mean;
							}else{
								it_main=it_back; it_main_mean=it_back_mean;
								it_main++; it_main_mean++;
								res.erase(it_back);	res_mean.erase(it_back_mean);
							}						
							new_main=true;
						}
						
					}else{
						it_tmp++;it_tmp_mean++;
					}
				}else{
					it_tmp++;it_tmp_mean++;
				}
			}
		}
		depth_old=res.depth(it);
	}
}

void remove_nonequiv_horiz ( tree<int>& res, tree<double>&res_mean, double vec_local_max_mean[], double vec_local_max_sd[] ){
	//remove horizontally the non-equivalent nodes
	tree<int>::pre_order_iterator it=res.begin();
	tree<double>::pre_order_iterator it_mean=res_mean.begin();
	tree<int>::leaf_iterator li;
	tree<double>::leaf_iterator li_mean;
	
	li=res.begin_leaf(it); li_mean=res_mean.begin_leaf(it_mean);
	while(li_mean!=res_mean.end_leaf(it_mean)) {
		if((*li_mean) < (vec_local_max_mean [res_mean.depth(li_mean)-1]- vec_local_max_sd [res_mean.depth(li_mean)-1]) ){
			res.erase(li);
			res_mean.erase(li_mean);
			li=res.begin_leaf(it);
			li_mean=res_mean.begin_leaf(it_mean);
		}else{
			++li;
			++li_mean;
		}
	}
}

void remove_childless_nodes( tree<int>& res, tree<double>&res_mean, int max_elements_tmp){
	tree<int>::pre_order_iterator it_tmp,it_back, it=res.begin();
	tree<double>::pre_order_iterator  it_mean_tmp,it_mean_back, it_mean=res_mean.begin();
	tree<int>::leaf_iterator li;
	tree<double>::leaf_iterator li_mean;
	bool found_child, multiple;
	
	int depth_max=0;
	//determine max depth
	while(it!=res.end()){
		if(depth_max<res.depth(it)){
			depth_max=res.depth(it);
		}
		it++;
	}
	it=res.begin();
	while(it!=res.end() && max_elements_tmp<= (depth_max+1)) {
		if(res.depth(it)<=(max_elements_tmp-2) && res.number_of_children(it)==0){ //advance through the tree
			it_tmp=res.parent(it);it_mean_tmp=res_mean.parent(it_mean);
			found_child=false; multiple=false;
			while(!found_child && (it_tmp!=res.begin() || res.number_of_children(it_tmp)>1)){ //end loop if there is a node with more than one child or if back at top and number of children==1 for top node
				if(res.number_of_children(it_tmp)==1){
					it_back=it_tmp;it_mean_back=it_mean_tmp; //if this was the last level for which there is only one child
					multiple=true;
					if(it_tmp!=res.begin()){
						it_tmp=res.parent(it_tmp); it_mean_tmp=res_mean.parent(it_mean_tmp);
					}else{//in case of having arrived at the top node
						res.erase(it_back); res_mean.erase(it_mean_back);
						found_child=true;
					}		
				}else{
					if(multiple){
						res.erase(it_back); res_mean.erase(it_mean_back);
					}else{
						res.erase(it); res_mean.erase(it_mean);
					}
					found_child=true;
				}
			}
			it=it_tmp; it_mean=it_mean_tmp;
		}else{
			++it; ++it_mean;
		}
	}	
}

double mrnet_onegene(double mim [], int size, int nbvar,int *var_ind,int var_target, int var_interest){
	// mim:			mutual information matrix
	// size:		total number of variables
	// nbvar:		number of previously selected variables (not the target)
	// var_ind:		the indices of the previously selected variables as vector
	// var_target:	the index of the target gene
	// var_interest: the variable for which the mrmr score with the target has to be computed; will be used for bootstrapping it
	
	unsigned int jmax;
	double rel, red,res;
	double max_val=-1000;
	
	jmax=var_target-1;
	//initialize the remaining entries to zero
	red=0;
	// the relevance for variable of interest with the target is simply its mutual information with it
	rel=mim[(var_target-1)*size+var_interest-1];
	if(nbvar > 0){
		// in case other variables have been previously selected; compute their redundancy with the variable of interest
		for(unsigned int j=0;j< nbvar; j++){
			red+=mim[(var_ind[j]-1)*size+var_interest-1];
		}		
		res=rel-red/nbvar ;
	}else{
		res=rel;
	}
	return res;
}

void bootstrap_mrmr(double &mean, double &sd, double data[],int size, int rep_boot, int size_boot,int nsamples, int var_target, int var_interest, int nprev_sel,int* var_ind)
{
	//mean
	//sd
	//data
	//size
	//rep_boot
	//size_boot
	//nsamples
	//var_target
	//var_interest
	//nprev_sel
	//var_ind

	int *ind;
	double *mim, *boot_val;
	
	ind=new int[size_boot];	
	mim = new double [size*size];
	boot_val=new double[rep_boot];
	
	
	for(unsigned int k=0; k< rep_boot; ++k){
	//in total there will be rep_boot times the mrmr sampled
		//determine the subset of samples that should be used (in total size_boot samples will be selected)
		for(unsigned int i=1;i<= size_boot;++i){
			ind[i-1]=rand () %nsamples;
		}
		// compute mi matrix for the subset
		build_mim_subset(mim, data, size, nsamples, ind, size_boot);
		boot_val[k]=mrnet_onegene( mim, size, nprev_sel, var_ind, var_target, var_interest);	
	}

	// determine mean and variance of bootstrapped values
	for(unsigned int i=0;i< rep_boot;++i){
		if(boot_val[i]==boot_val[i]){
			mean+=boot_val[i];
		}
	}
	mean=mean/rep_boot;

	for(unsigned int i=0;i< rep_boot;++i){
		if(boot_val[i]==boot_val[i]){
			sd+=(boot_val[i]-mean) * (boot_val[i]-mean);
		}
	}
	sd=sqrt(sd/rep_boot);
	
	delete [] ind;
	delete [] mim;
	delete [] boot_val;
}	
void mrmr_ensemble_one_gene (tree<int>& res, tree<int>::pre_order_iterator one, double data[], int nsamples,int n , int max_elements, int predn , int rep_boot, int maxnsol, double threshold){
	//n					number of variables
	//predn:			index of target node
	
	// nsub: the variables which have been previously selected + target; prev_sel=nsub-target
	// number of samples to use for bootstrapping is equal to total number of samples
	int  *nsub, *prev_sel,nsamples_boot=nsamples, nprev_sel=0; 
	double *res_vec, *vec_mean, *vec_sort, *vec_sd,  *vec_local_max_mean, *vec_local_max_sd;;
	
	double max_mrmr;
	int rootdepth, max_mrmr_ind,cnt=0, max_elements_tmp=1; //current depth in the tree
	bool notincremented;
	
	res_vec=new double [n];
	vec_mean=new double [n];
	vec_sd=new double [n];
	vec_sort=new double [n];
	
	vec_local_max_mean=new double [max_elements];
	vec_local_max_sd=new double [max_elements];
	
	for(unsigned int k=0;k< max_elements ;++k){
		vec_local_max_mean[k]=-1000;
	}
	
	prev_sel=new int[max_elements];
	nsub = new int [max_elements];
	
	tree<double> res_mean;
	tree<double>::iterator top_mean,one_mean;
	
	//initialize tree: res_mean for mean of the bootstrap
	top_mean=res_mean.begin();
	one_mean=res_mean.insert(top_mean, predn);
	
	
	//mrmr score should not be predicted to the target node
	vec_mean[predn-1]=-1000; vec_sd[predn-1]=-1000; max_mrmr=-1000.0; max_mrmr_ind=-1;notincremented=true;
	prev_sel[0]=0; nsub[0]=predn;
	
	while (max_elements_tmp<=max_elements) {
			
		//initialize different iterators for the two trees
		tree<int>::pre_order_iterator it=res.begin();
		tree<double>::pre_order_iterator it_mean=res_mean.begin(), it_mean_tmp=res_mean.begin();
		
		tree<int>::leaf_iterator li;
		tree<double>::leaf_iterator li_mean;
		rootdepth=res.depth(it);
		
		while(it!=res.end()) {
			if(!notincremented ){	
				li=res.begin_leaf(it);
				li_mean=res_mean.begin_leaf(it_mean);
				while(li_mean!=res_mean.end_leaf(it_mean_tmp)) {
					//delete leafs for which the local mrmr score is not in the confidence interval
					if((*li_mean) < (vec_local_max_mean [res_mean.depth(li_mean)-1]- vec_local_max_sd [res_mean.depth(li_mean)-1]) ){
						res.erase(li);
						res_mean.erase(li_mean);
						li=res.begin_leaf(it);
						li_mean=res_mean.begin_leaf(it_mean);
					}else{
						++li;
						++li_mean;
					}
				}
				remove_childless_nodes( res, res_mean, max_elements_tmp);
				notincremented=true;
				cnt++;
				
			}
			//if actually in the next level
			if(res.depth(it)-rootdepth==max_elements_tmp-1){
				if(cnt!=0){
					nsub[res.depth(it)-1]=*res.parent(it);   //previous node did not have any children, should be replaced by actual parent; same parent is the same for all siblings
					nsub[res.depth(it)]=(*it) ;
					
					//add all previously selected variables to prev_sel -> needed in bootstrapping function
					for (unsigned int i=0;i<max_elements_tmp-1;++i){	
						prev_sel[i]=nsub[i+1];					
					}
				}
				////////////
				// initialize vec_mean and vec_sd for bootstrapping to -1000 if variable is not supposed to be tested (target or prev selected) otherwise 0
				////////////
				for(unsigned int k=0;k< n;++k){
					vec_mean[k]=0;vec_sd[k]=0;
				}
				for(unsigned int k=0;k<max_elements_tmp;++k){
					vec_mean[nsub[k]-1]=-1000;	vec_sd[nsub[k]-1]=-1000;
				}
				max_mrmr=-1000.0;max_mrmr_ind=-1;
				for(unsigned int k=0;k< n;++k){
					if(vec_mean[k]!= (-1000)){
						bootstrap_mrmr(vec_mean[k],vec_sd[k], data,n, rep_boot,nsamples_boot,nsamples,nsub[0],(k+1), max_elements_tmp-1,prev_sel);				
					}	
					vec_sort[k]=vec_mean[k];
					if(k!=(nsub[0]-1) && vec_mean[k]>max_mrmr){
						max_mrmr=vec_mean[k];max_mrmr_ind=k;
					}
				}
				///add the maxnsol equivalently good nodes as children				
				if(max_mrmr>threshold){ 
					sort(vec_sort,vec_sort+n);
					if( max_mrmr > vec_local_max_mean[res.depth(it)]){
						vec_local_max_mean[res.depth(it)]=max_mrmr;
						vec_local_max_sd[res.depth(it)]=vec_sd[max_mrmr_ind];
					}
					//append equivalently good nodes to the two trees
					for(unsigned int k=0;k< n;++k){
						if(vec_mean[k]>vec_sort[n-(maxnsol)-1] && vec_mean[k]<= vec_mean[max_mrmr_ind]+vec_sd[max_mrmr_ind] && vec_mean[k]>= vec_mean[max_mrmr_ind]-vec_sd[max_mrmr_ind]){
							res.append_child(it,k+1); res_mean.append_child(it_mean,vec_mean[k]);
						}
					}
				}
			}else{
				nsub[res.depth(it)]=(*it) ;
			}
			++it; ++it_mean;
			li=res.begin_leaf(it); li_mean=res_mean.begin_leaf(it_mean);
		}
		remove_equiv_subtrees(res,res_mean);
		cnt++; max_elements_tmp++; notincremented=false;		
	}
	remove_nonequiv_horiz(res,res_mean,vec_local_max_mean, vec_local_max_sd);
	remove_childless_nodes(res, res_mean,max_elements_tmp);
	
}

SEXP mrmr_ensemble( SEXP Rdata, SEXP Rmaxparents, SEXP Rnvar, SEXP Rnsample, SEXP Rpredn, SEXP Rnpredn, SEXP Rrep_boot, SEXP Rmaxnsol, SEXP Rthreshold){
	// Rdata:		data should be passed as vector, variable-wise appended
	// Rmaxparents:	number of maximum number of parents
	// Rnvar:		number of variables in the dataset
	// Rnsample:	number of samples in the dataset
	// Rpredn:		vector of target genes to consider
	// Rnpredn:		number of target genes (number of elements in Rpredn)
	// Rrep_boot:	how many bootstrap iterations
	// Rmaxnsol:	maximum number of children for each node at each step
	
	double *data, *threshold;
	const int* maxparents, * nvar, *nsample, *maxnsol;
	
	int *predn, *rep_boot,*res,*res_all,*res_all2;
	int vec_tmp;
	const int *npredn;
	
	SEXP Rres;
	
	srand (time(NULL));
	PROTECT(Rdata = AS_NUMERIC(Rdata));
	PROTECT(Rmaxparents= AS_INTEGER(Rmaxparents));
	PROTECT(Rnvar= AS_INTEGER(Rnvar));	
	PROTECT(Rnsample= AS_INTEGER(Rnsample));
	PROTECT(Rpredn = AS_INTEGER(Rpredn));
	PROTECT(Rnpredn = AS_INTEGER(Rnpredn));
	PROTECT(Rrep_boot = AS_INTEGER(Rrep_boot));
	PROTECT(Rmaxnsol= AS_INTEGER(Rmaxnsol));
	PROTECT(Rthreshold= AS_NUMERIC(Rthreshold));
	
	data=NUMERIC_POINTER(Rdata);
	maxparents = INTEGER_POINTER(Rmaxparents);
	nvar= INTEGER_POINTER(Rnvar);
	nsample= INTEGER_POINTER(Rnsample);
	predn= INTEGER_POINTER(Rpredn);
	npredn= INTEGER_POINTER(Rnpredn);
	rep_boot= INTEGER_POINTER(Rrep_boot);	
	maxnsol= INTEGER_POINTER(Rmaxnsol);
	threshold = NUMERIC_POINTER(Rthreshold);
	
	tree<int> res_tree;
	tree<int>::iterator top,one;
	tree<int>::breadth_first_queued_iterator it_final;
	
	top=res_tree.begin();
	
	int length_res=0;
	int length_res_old;
	
	for(unsigned int i=0;i< *npredn;++i){
		//initialize tree
		std::cout<<"model for node "<<predn[i]<< " is being built!"<<std::endl;
		one=res_tree.insert(top, predn[i]);
		
		//build ensemble tree
		mrmr_ensemble_one_gene(res_tree, one, data,*nsample,*nvar,*maxparents,predn[i],*rep_boot, *maxnsol, *threshold);
		
		////////////////////////
		//convert tree to vector
		////////////////////////
		int *tmp_nchildren,*res_tmp;
		res_tmp=new int [2*(res_tree.size())+1];
		tmp_nchildren= new int [(res_tree.size())];
		
		it_final=res_tree.begin_breadth_first();
		int cnt=1,cnt2=0;
		
		res_tmp[0]=res_tree.size();
		int rootdepth=res_tree.depth(it_final);
		while(it_final!=res_tree.end_breadth_first()) {
			res_tmp[cnt]=*it_final;
			tmp_nchildren[cnt-1]=(res_tree.number_of_children(it_final));
			cnt++;
			++it_final;		
		}
		
		////////////////
		//save in final result vector
		////////////////
		length_res_old=length_res;
		length_res+=2*(res_tree.size())+1;
		int *res_all, *res_old;
		int ind=0;
		res_all=new int[length_res];
		if(length_res_old>0){
			for(unsigned int k=0;k<length_res_old;k++){
				res_all[k]=res_old[k];
			}
		}
		
		for(unsigned int k=0;k<=res_tree.size();k++){
			res_all[length_res_old+k]=res_tmp[k];
		}
		for(unsigned int k=0;k<res_tree.size();k++){
			res_all[length_res_old+k+res_tree.size()+1]=tmp_nchildren[k];
		}
		
		delete [] res_old;
		res_old=new int[length_res];
		for(unsigned int k=0;k<length_res;k++){
			res_old[k]=res_all[k];
		}
		
		delete [] res_all;
		if(i==(*npredn-1)){
			PROTECT(Rres = NEW_INTEGER(length_res));
			res = INTEGER_POINTER(Rres);
			for(unsigned int k=0;k<length_res;k++){
				res[k]=res_old[k];
			}
			delete [] res_old;
		}
		
		////////////////
		//erase old tree
		////////////////
		
		delete [] tmp_nchildren;
		delete [] res_tmp;
		res_tree.erase(one);
	}
	
	
	UNPROTECT(10);
	
	return Rres;
}
