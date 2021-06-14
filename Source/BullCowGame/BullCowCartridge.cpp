#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("HiddenWordLists/WordList.txt");
    FFileHelper::LoadFileToStringArray(HiddenWords, *WordListPath);

    Isograms = GetValidWords(HiddenWords);

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        ProcessGuess(PlayerInput);    
    }
}

void UBullCowCartridge::SetupGame()
{
    //Welcome Player and give instruction
    PrintLine(TEXT("Hello There\n"));
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    PrintLine(TEXT("You will be guessing a %i letter isogram.\n"), HiddenWord.Len());
    PrintLine(TEXT("The Hidden Word is: %s"), *HiddenWord); //Debug Line
    PrintLine(TEXT("You have %i lives."), HiddenWord.Len());
    PrintLine(TEXT("Type in your guess and press enter..."));
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("You've won!"));
        EndGame();
        return;
    }

    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("You've entered the wrong number of letters\nThe hidden word is %i letters long."), HiddenWord.Len());
        return;
    }

    if (!IsIsogram(Guess))
    {
       PrintLine(TEXT("No repeating letters!"));
       return;
    }

    //if all validation pass but incorrect word
    PrintLine(TEXT("Sorry, you've lost a life.\nTry again. Lives remaining: %i"), --Lives);
    
    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("All lives have been lost, game over!\nThe hidden word was: %s"), *HiddenWord);
        PrintLine(TEXT("\nPress enter to continue."));
        EndGame();
        return;
    }
    //Show Bulls and Cows
    FBullCowCount Score = GetBullCows(Guess);
    PrintLine(TEXT("You got %i Bulls and %i Cows"), Score.Bulls, Score.Cows);
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const
{
    for (int32 Index = 0; Index < Word.Len(); Index++)
    {
        for (int32 Comparison = Index + 1; Comparison < Word.Len(); Comparison++)
        {
            if (Word[Index] == Word[Comparison])
            {
                return false;
            }
        }
    }
    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
    TArray<FString> ValidWords;

    for (FString Word : WordList)
    {
      if (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))
        {
            ValidWords.Emplace(Word);
        }  
    }
    return ValidWords;
} 

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }

        for (int32 CowIndex = 0; CowIndex < HiddenWord.Len(); CowIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[CowIndex])
            {
                Count.Cows++;
                break;
            }
            
        }
        
    }
    return Count;
}