// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "K2Node.h"
#include "K2Node_Step.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3EDITOR_API UK2Node_Step : public UK2Node
{
	GENERATED_BODY()

private:

	// Pin Names

	static const FName PN_StatementIn;
	static const FName PN_Finalize;
	static const FName PN_Break;

	static const FName PN_ForEachRow;
	static const FName PN_StatementOut;
	static const FName PN_Done;
	static const FName PN_Error;
	static const FName PN_ReturnCode;

	[[nodiscard]] bool CheckForErrors( const FKismetCompilerContext& CompilerContext );

public:
	// Pin Accessors

	[[nodiscard]] UEdGraphPin* GetStatementInPin( void ) const;
	[[nodiscard]] UEdGraphPin* GetFinalizePin( void ) const;
	[[nodiscard]] UEdGraphPin* GetBreakPin( void ) const;

	[[nodiscard]] UEdGraphPin* GetForEachPin( void ) const;
	[[nodiscard]] UEdGraphPin* GetStatementOutPin( void ) const;
	[[nodiscard]] UEdGraphPin* GetDonePin( void ) const;
	[[nodiscard]] UEdGraphPin* GetErrorPin( void ) const;
	[[nodiscard]] UEdGraphPin* GetReturnCodePin( void ) const;

	// K2Node API

	[[nodiscard]] bool IsNodeSafeToIgnore() const override { return true; }

	void GetMenuActions( FBlueprintActionDatabaseRegistrar& ActionRegistrar ) const override;

	[[nodiscard]] FText GetMenuCategory() const override;

	// EdGraphNode API

	void AllocateDefaultPins() override;

	void ExpandNode( FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph ) override;

	[[nodiscard]] FText GetNodeTitle( ENodeTitleType::Type TitleType ) const override;

	[[nodiscard]] FText GetTooltipText() const override;

	[[nodiscard]] FSlateIcon GetIconAndTint( FLinearColor& OutColor ) const override;

	bool ShouldShowNodeProperties() const override { return true; }
};
