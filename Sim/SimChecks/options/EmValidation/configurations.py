##########################################################################################################
#
# EM physics study and validation with EMGaussMoni and BremVeloCheck. Configurations that hold information
# for each job.
#
#  Georgios Chatzikonstantinidis 14/05/2017
#  email:georgios.chatzikonstantinidis@cern.ch
#
##########################################################################################################
def config():
	pgunID = 11
	pgunE = 0.1
	emPL = 'NoCuts'
	return {'veloType':'velo','pgunID':pgunID,'pgunE':pgunE,'nEvts': 1000,'dRays':False,'emPL':emPL,
		'saveSim':False,'runGenerator':True,'testType':'both'}
