#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sndfile.h>

const double PI = 3.141592653589793;

// Função para modulação AM
std::vector<double> modularAM(const std::vector<double>& sinal, double freqPortadora, double amostragem) {
    std::vector<double> sinalModulado(sinal.size());
    for (size_t i = 0; i < sinal.size(); ++i) {
        double t = i / amostragem;
        sinalModulado[i] = sinal[i] * cos(2 * PI * freqPortadora * t);
    }
    return sinalModulado;
}

// Adicionar ruído ao sinal
std::vector<double> adicionarRuido(const std::vector<double>& sinal, double intensidadeRuido) {
    std::vector<double> sinalRuido = sinal;
    srand(static_cast<unsigned>(time(0)));
    for (double& amostra : sinalRuido) {
        amostra += ((rand() / static_cast<double>(RAND_MAX)) * 2 - 1) * intensidadeRuido;
    }
    return sinalRuido;
}

// Função para demodular AM
std::vector<double> demodularAM(const std::vector<double>& sinal, double freqPortadora, double amostragem) {
    std::vector<double> sinalDemodulado(sinal.size());
    for (size_t i = 0; i < sinal.size(); ++i) {
        double t = i / amostragem;
        sinalDemodulado[i] = sinal[i] * cos(2 * PI * freqPortadora * t);
    }
    return sinalDemodulado;
}

// Normalizar o sinal para salvar como áudio
void normalizar(std::vector<double>& sinal, double maxValor) {
    double maxAtual = 0.0;
    for (double v : sinal) {
        if (std::abs(v) > maxAtual) maxAtual = std::abs(v);
    }
    if (maxAtual > 0) {
        for (double& v : sinal) {
            v = (v / maxAtual) * maxValor;
        }
    }
}

int main() {
    // Abrir arquivo de entrada
    const char* arquivoEntrada = "/home/matheus/Downloads/Phil-Collins-Another-Day-In-Paradise-_Audio-HQ_-HD.wav";
    const char* arquivoSaida = "am_audio_receptor.wav";

    SF_INFO sfInfoEntrada;
    SNDFILE* arquivoAudioEntrada = sf_open(arquivoEntrada, SFM_READ, &sfInfoEntrada);
    if (!arquivoAudioEntrada) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << sf_strerror(nullptr) << std::endl;
        return 1;
    }

    // Ler amostras do áudio
    std::vector<double> sinalOriginal(sfInfoEntrada.frames);
    sf_read_double(arquivoAudioEntrada, sinalOriginal.data(), sfInfoEntrada.frames);
    sf_close(arquivoAudioEntrada);

    // Parâmetros da simulação
    double freqPortadora = 64000.0; // Frequência da portadora (Hz)
    double intensidadeRuido = 0.05; // Intensidade do ruído

    // Modulação
    auto sinalModulado = modularAM(sinalOriginal, freqPortadora, sfInfoEntrada.samplerate);

    // Transmissão com ruído
    auto sinalTransmitido = adicionarRuido(sinalModulado, intensidadeRuido);

    // Demodulação
    auto sinalRecebido = demodularAM(sinalTransmitido, freqPortadora, sfInfoEntrada.samplerate);

    // Normalizar o sinal recebido para salvar como áudio
    normalizar(sinalRecebido, 0.4);

    // Gravar o arquivo de saída
    SF_INFO sfInfoSaida = sfInfoEntrada;
    SNDFILE* arquivoAudioSaida = sf_open(arquivoSaida, SFM_WRITE, &sfInfoSaida);
    if (!arquivoAudioSaida) {
        std::cerr << "Erro ao criar o arquivo de saída: " << sf_strerror(nullptr) << std::endl;
        return 1;
    }
    sf_write_double(arquivoAudioSaida, sinalRecebido.data(), sinalRecebido.size());
    sf_close(arquivoAudioSaida);

    std::cout << "Simulação concluída. Arquivo recebido salvo em: " << arquivoSaida << std::endl;
    return 0;
}
//g++ -o am_modulation am_modulation.cpp -lsndfile -Wall -Wextra -O2

