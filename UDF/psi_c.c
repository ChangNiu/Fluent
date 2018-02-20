#include "udf.h"

static int version = 1;
static int release = 2;


DEFINE_EXECUTE_ON_LOADING(report_version, libname)
{
    Message("\nLoading %s version %d.%d\n",libname,version,release);
	
	Set_User_Memory_Name(0,"DPM_CON");
	Set_User_Memory_Name(1,"PSI_C");
}


DEFINE_ON_DEMAND(on_demand_calc)
{
   Domain *d; /* declare domain pointer since it is not passed as an  
                 argument to the DEFINE macro  */
   Thread *t;
   cell_t c;
   
   d = Get_Domain(1);     /* Get the domain using Fluent utility */

   /* Loop over all cell threads in the domain */
   thread_loop_c(t,d)
   {	 
		/* Loop over all cells  */
		begin_c_loop(c,t)
		{
		   C_UDMI(c,t,0) = 0.0;
		   C_UDMI(c,t,1) = 0.0;
		}
		end_c_loop(c,t)
 
   }
      
   Message("\nUser Defined Memory is now initialized!\n"); 
}

DEFINE_ADJUST(adjust_dpm, d)
{
	Thread *t;
	cell_t c;
	
	thread_loop_c(t,d)
	{
		begin_c_loop(c,t)
		{
			/* C_UDMI(c,t,0) = C_DPMS_CONCENTRATION(c,t); */
		}
		end_c_loop(c,t)
	}
	
	printf("Adjust DPM Mass Concentration\n");
	fflush(stdout);
}

DEFINE_EXECUTE_AT_END(execute_at_end)
{

	Domain *d;
	Thread *t;
	cell_t c;
	d = Get_Domain(1);   /* mixture domain if multiphase */

	thread_loop_c(t,d)
	{
		begin_c_loop(c,t)
		{
			C_UDMI(c,t,0) += C_DPMS_CONCENTRATION(c,t);
		}
		end_c_loop(c,t)
	}

	printf("Update DPM Mass Concentration\n");
	fflush(stdout);
}

DEFINE_DPM_SCALAR_UPDATE(Part_Con, c, t, initialize, p)
{

	C_UDMI(c,t,1) += P_FLOW_RATE(p)*P_DT(p)/C_VOLUME(c,t);
}