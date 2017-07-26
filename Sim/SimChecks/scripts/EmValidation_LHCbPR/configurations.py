##########################################################################################################
#
# EM physics study and validation with EMGaussMoni and BremVeloCheck. Configurations that hold information
# for each job.
#
#  Peter Griffith 21/11/2014
#  email:peter.griffith@cern.ch
#
#  Georgios Chatzikonstantinidis 14/05/2017
#  email:georgios.chatzikonstantinidis@cern.ch
#
##########################################################################################################
def config():
	pgunID = 11
	pgunE = 0.1
	emPL = 'NoCuts'
	return {'veloType':'velo','pgunID':pgunID,'pgunE':pgunE,'nEvts': 100,'dRays':False,'emPL':emPL,
		'saveSim':False,'runGenerator':True,'testType':'both'}
