from random import randint

for i in range(0, 100):
	with open('./samples_a_n_b_n/{}.txt'.format(i), 'w') as f:
		s = ''
		num = randint(0, 100)
		s = ('a' * num) + ('b' * num)
		f.write(s)