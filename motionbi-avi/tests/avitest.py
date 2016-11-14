#!/bin/python

from subprocess import Popen, call
from time import sleep
from math import radians
import socket
from struct import pack

settings = {
    'exe_path' : 'bin/gprs',
    'port' : 12345,
    'host'   : '127.0.0.1',

    #Event types
    'EV_CAMERAMOVE'   : 1,
    'EV_SELECTION'    : 5,
    'EV_WORLDROTATE'  : 6,
    'EV_GRAPHOVERLAP' : 7,
}

state = {
    'proc' : None,
    'socket' : socket.socket(socket.AF_INET, socket.SOCK_DGRAM),
}

scores = ['correto', 'impreciso', 'incorreto']

def init():
    global tests
    tests = [
    {
    'id' : 'MBI-ST25AVI',
    'func' : ST25AVI,
    'description' : 'Envio de pacotes de movimentação absoluta do ponto de vista de visualização.',
    'expected_result' : 'Descrito passo a passo no output do script executado. Em caso de falha num passo indicar o número do mesmo nas observações do relatório de teste.',
    },
    {
    'id' : 'MBI-ST26AVI',
    'func' : ST26AVI,
    'description' : 'Envio de pacotes para mudança do modo de visualização (tudo, colunas, linhas e barras).',
    'expected_result' : 'Descrito passo a passo no output do script executado. Em caso de falha num passo indicar o número do mesmo nas observações do relatório de teste.',
    },
    {
    'id' : 'MBI-ST27AVI',
    'func' : ST27AVI,
    'description' : 'Envio de pacotes para alteração da linha/coluna/barra selecionada.',
    'expected_result' : 'Descrito passo a passo no output do script executado. Em caso de falha num passo indicar o número do mesmo nas observações do relatório de teste.',
    },
    {
    'id' : 'MBI-ST28AVI',
    'func' : ST28AVI,
    'description' : 'Pacotes de rotação para cada gráfico.',
    'expected_result' : 'Descrito passo a passo no output do script executado. Em caso de falha num passo indicar o número do mesmo nas observações do relatório de teste.',
    },
    {
    'id' : 'MBI-ST29AVI',
    'func' : ST29AVI,
    'description' : 'Pacotes para junção e separação dos gráficos.',
    'expected_result' : 'Descrito passo a passo no output do script executado. Em caso de falha num passo indicar o número do mesmo nas observações do relatório de teste.',
    }]

def restart():
    terminate()
    sleep(0.4)
    with open('/dev/null', 'w') as devnull:
        state['proc'] = Popen(settings['exe_path'], stdout=devnull, stderr=devnull)

def terminate():
    if( state['proc'] != None ):
        call(['kill', str(state['proc'].pid)])
        state['proc'] = None

def send(packet):
    state['socket'].sendto(packet, (settings['host'], settings['port']))

def await_input():
    print('[pressiona <enter> para avançar]')
    input()

#Envio de pacotes de movimentação absoluta do ponto de vista de visualização.
def ST25AVI():
    results = []
    step = 0

    for x in [0, 1]:
        for y in [0, 1]:
            for z in [0, 1]:
                if( x == 0 and y == 0 and z == 0):
                    continue
                restart()
                step += 1
                print('\n\nPASSO #%d => Vou colocar a camera na posição (%d, %d, %d)'%(step,x,y,z))
                await_input()
                send( pack('iifffff', settings['EV_CAMERAMOVE'], 1, 0, 0, x, y, z) )
                print('Resultado Esperado: A camera está na posição x=%d, y=%d, z=%d a olhar para a origem'%(x,y,z))
                print('Qual foi o resultado?')
                results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    return results

#Envio de pacotes para mudança do modo de visualização (tudo, colunas, linhas e barras).
def ST26AVI():

    results = []
    step = 0
    modestr = ['tudo', 'colunas', 'linhas', 'barras']

    for graph in range(0,2):
        for mode in range(2,5):
            restart()
            step += 1
            print('\n\nPASSO #%d => Vou enviar um pacote para mudar o gráfica da %s para modo \'%s\'.'%(step, 'esquerda' if graph == 0 else 'direita', modestr[mode-1]) )
            await_input()
            send( pack('iiiii', settings['EV_SELECTION'], mode, 0, 0, graph) )
            print('O gráfico da %s ficou em no modo de visualização \'%s\''%('esquerda' if graph == 0 else 'direita', modestr[mode-1]) )
            print('Qual foi o resultado?')
            results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )
    
    return results

#Envio de pacotes para alteração da linha/coluna/barra selecionada.
def ST27AVI():
    results = []
    modestr = ['tudo', 'colunas', 'linhas', 'barras']

    restart()
    print('\n\nPASSO #1 => Vou enviar um pacote para avançar a linha selectionada do gráfico da esquerda')
    await_input()
    send( pack('iiiii', settings['EV_SELECTION'], 3, 0, 1, 0))
    print('A linha selecionada no gráfico da esquerda avançou uma posição em profundidade')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    restart()
    print('\n\nPASSO #2 => Vou enviar um pacote para avançar a coluna selectionada do gráfico da esquerda')
    await_input()
    send( pack('iiiii', settings['EV_SELECTION'], 2, 1, 0, 0))
    print('A coluna selecionada no gráfico da esquerda avançou uma posição')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    restart()
    print('\n\nPASSO #3 => Vou enviar um pacote para avançar a barra selectionada do gráfico da direita na diagonal')
    await_input()
    send( pack('iiiii', settings['EV_SELECTION'], 4, 1, 1, 1))
    print('A barra selecionada no gráfico da direita avançou uma posição em profundidade e em largura')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    restart()
    print('\n\nPASSO #4 => Vou enviar um pacote para avançar a barra selectionada do gráfico da direita na diagonal')
    print('\tDepois, vou enviar um pacote para colocar a barra selectionada no seu sitio original.')
    await_input()
    send( pack('iiiii', settings['EV_SELECTION'], 4, 1, 1, 1))
    send( pack('iiiii', settings['EV_SELECTION'], 4, -1, -1, 1))
    print('No final, a barra selecionada no gráfico da direita continuou a ser a mesma.')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    return results

#Pacotes de rotação para cada gráfico.
def ST28AVI():
    
    graph_label = ['esquerda', 'direita']
    results = []
    step = 0

    for graph in [0,1]:
        for angle in [[20,20,5], [-20,-20,5,-10]]:

            restart()
            step += 1
            print('\n\nPASSO #%d => Vou enviar um pacote para rodar o gráfico da %s %d graus.'%(step, graph_label[graph], sum(angle)))
            await_input()

            for ang in angle:
                send( pack('iiffffi', settings['EV_WORLDROTATE'], 0, radians(ang), 0, 0, 0, graph) )

            print('No final, o gráfico da %s ficou rodado %d graus.'%(graph_label[graph], sum(angle)))
            print('Qual foi o resultado?')
            results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )
    return results

#Pacotes para junção e separação dos gráficos.
def ST29AVI():

    results = []
    restart()

    print('\n\nPASSO #1 => Vou enviar um pacote para juntar os dois gráficos')
    await_input()
    send( pack('ii', settings['EV_GRAPHOVERLAP'], 1) )
    print('Os gráficos juntaram da forma descrita no documento de arquitectura')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    print('\n\nPASSO #1 => Vou enviar um pacote para voltar a separar os dois gráficos')
    await_input()
    send( pack('ii', settings['EV_GRAPHOVERLAP'], 0) )
    print('Os gráficos voltaram ao seu estado inicial')
    print('Qual foi o resultado?')
    results.append( int(input('introduz [1] para correto, [2] para impreciso ou [3] para incorreto')) )

    return results

if __name__ == '__main__':
    init()

    for test in tests:
        
        print('[ID] ' + test['id'])
        print('[Descrição]\n' + test['description'])
        print('[Resultado Esperado]\n' + test['expected_result'])
        test['result'] = test['func']()

        print('Teste terminado. pressiona <enter> para seguir ao próximo teste.')
        await_input()

    for test in tests:
        out = test['id'] + '\t' + scores[ max(test['result'])-1 ]
        if( max(test['result']) == 3 ):
            print(out + '\tPassos Incorretos: ' + ' '.join([ str(i+1) for i in range(len(test['result'])) if test['result'][i] == max(test['result']) ]))
        else:
            print(out)

