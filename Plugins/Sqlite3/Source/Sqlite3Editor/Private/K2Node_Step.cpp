// (c)2024+ Laurent Menten


#include "K2Node_Step.h"
#include "Sqlite3Editor.h"
#include "SqliteStatement.h"

#include "K2Utils.h"

// BlueprintGraph
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_Knot.h"

// Kismet
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// KismetCompiler
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "FSqlite3BlueprintModule"

#define GETENUMSTRING( etype, evalue ) \
	( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) \
		? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameByIndex((int32)evalue).ToString() \
		: FString("Could not find UENUM type '" etype "'") )

// ============================================================================
// = Pin names ================================================================
// ============================================================================

// Input

const FName UK2Node_Step::PN_StatementIn( TEXT( "StatementInPin" ) );
const FName UK2Node_Step::PN_Finalize( TEXT( "FinalizePin" ) );
const FName UK2Node_Step::PN_Break( TEXT( "BreakPin" ) );

// Output

const FName UK2Node_Step::PN_ForEachRow( UEdGraphSchema_K2::PN_Then );
const FName UK2Node_Step::PN_StatementOut( TEXT( "StatementOutPin" ) );
const FName UK2Node_Step::PN_Done( TEXT( "DonePin" ) );
const FName UK2Node_Step::PN_Error( TEXT( "ErrorPin" ) );
const FName UK2Node_Step::PN_ReturnCode( TEXT( "ReturnCodePin" ) );

// ============================================================================
// = 
// ============================================================================

void UK2Node_Step::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// ---------------------------------------------------------------------------
	//  - Execution pin
	// ---------------------------------------------------------------------------

	CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute );

	// ---------------------------------------------------------------------------
	// - Statement input pin -----------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto StatementInPin = CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Object, USqliteStatement::StaticClass(), PN_StatementIn );
	StatementInPin->PinFriendlyName = LOCTEXT( "StatementInPin_FriendlyName", "Statement (in)" );
	K2Utils::SetPinToolTip( StatementInPin, LOCTEXT( "StatementInPin_Tooltip", "Statement to process" ) );

	// ---------------------------------------------------------------------------
	// - Finalize input pin ------------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto FinalizePin = CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Boolean, PN_Finalize );
	FinalizePin->PinFriendlyName = LOCTEXT( "FinalizePin_FriendlyName", "Finalize" );
	K2Utils::SetPinToolTip( FinalizePin, LOCTEXT( "FinalizePin_Tooltip", "Finalize statement when done" ) );

	// ---------------------------------------------------------------------------
	// - Break input pin ---------------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto BreakPin = CreatePin( EGPD_Input, UEdGraphSchema_K2::PC_Exec, PN_Break );
	BreakPin->PinFriendlyName = LOCTEXT( "BreakPin_FriendlyName", "Break" );

	// ---------------------------------------------------------------------------
	// - ForEachRow execution pin ------------------------------------------------
	// ---------------------------------------------------------------------------

	// For Each pin
	const auto ForEachPin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Exec, PN_ForEachRow );
	ForEachPin->PinFriendlyName = LOCTEXT( "ForEachPin_FriendlyName", "For each row" );

	// ---------------------------------------------------------------------------
	// - Statement output pin ----------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto StatementOutPin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Object, USqliteStatement::StaticClass(), PN_StatementOut );
	StatementOutPin->PinFriendlyName = LOCTEXT( "StatementOutPin_FriendlyName", "Statement (out)" );
	K2Utils::SetPinToolTip( StatementOutPin, LOCTEXT( "StatementOutPin_Tooltip", "Statement being processed" ) );

	// ---------------------------------------------------------------------------
	// - Done execution pin ------------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto DonePin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Exec, PN_Done );
	DonePin->PinFriendlyName = LOCTEXT( "DonePin_FriendlyName", "Done" );
	DonePin->PinToolTip = LOCTEXT( "DonePin_Tooltip", "Execution once all rows have been processed" ).ToString();

	// ---------------------------------------------------------------------------
	// - Error execution pin -----------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto ErrorPin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Exec, PN_Error );
	ErrorPin->PinFriendlyName = LOCTEXT( "ErrorPin_FriendlyName", "Error" );
	ErrorPin->PinToolTip = LOCTEXT( "ErrorPin_Tooltip", "Execution on error" ).ToString();

	// ---------------------------------------------------------------------------
	// - ReturnCode output pin ---------------------------------------------------
	// ---------------------------------------------------------------------------

	const auto ReturnCodePin = CreatePin( EGPD_Output, UEdGraphSchema_K2::PC_Int, PN_ReturnCode );
	ReturnCodePin->PinFriendlyName = LOCTEXT( "ReturnCodePin_FriendlyName", "Return code" );
	K2Utils::SetPinToolTip( ReturnCodePin, LOCTEXT( "ReturnCodePin_Tooltip", "SQLITE return code" ) );
}

// ============================================================================
// = Generate subgraph ========================================================
// ============================================================================

void UK2Node_Step::ExpandNode( FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph )
{
	Super::ExpandNode( CompilerContext, SourceGraph );

	const auto K2Schema = GetDefault<UEdGraphSchema_K2>();

	// ------------------------------------------------------------------------
	// - Error handling -------------------------------------------------------
	// ------------------------------------------------------------------------

	if( CheckForErrors( CompilerContext ) )
	{
		BreakAllNodeLinks();
		return;
	}

	// ------------------------------------------------------------------------
	// - Get custom node pins -------------------------------------------------
	// ------------------------------------------------------------------------

	const auto ExecPin = GetExecPin();
	const auto StatementInPin = GetStatementInPin();
	const auto FinalizePin = GetFinalizePin();
	const auto BreakPin = GetBreakPin();

	const auto ForEachPin = GetForEachPin();
	const auto StatementOutPin = GetStatementOutPin();
	const auto DonePin = GetDonePin();
	const auto ErrorPin = GetErrorPin();
	const auto ReturnCodePin = GetReturnCodePin();

	// ------------------------------------------------------------------------
	// - Statement variable ---------------------------------------------------
	// ------------------------------------------------------------------------

	const auto StatementVariable = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>( this, SourceGraph );
	StatementVariable->VariableType.PinCategory = UEdGraphSchema_K2::PC_Object;
	StatementVariable->VariableType.PinSubCategoryObject = USqliteStatement::StaticClass();
	StatementVariable->AllocateDefaultPins();

	const auto StatementVariable_Variable = StatementVariable->GetVariablePin();
	StatementVariable_Variable->DefaultValue = TEXT( "None" );

	// ------------------------------------------------------------------------
	// - Break variable -------------------------------------------------------
	// ------------------------------------------------------------------------

	const auto BreakVariable = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>( this, SourceGraph );
	BreakVariable->VariableType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	BreakVariable->AllocateDefaultPins();

	const auto BreakVariable_Variable = BreakVariable->GetVariablePin();
	BreakVariable_Variable->DefaultValue = TEXT( "0" );

	// ------------------------------------------------------------------------
	// - ReturnCode variable --------------------------------------------------
	// ------------------------------------------------------------------------

	const auto ReturnCodeVariable = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>( this, SourceGraph );
	ReturnCodeVariable->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;
	ReturnCodeVariable->AllocateDefaultPins();

	const auto ReturnCodeVariable_Variable = ReturnCodeVariable->GetVariablePin();
	ReturnCodeVariable_Variable->DefaultValue = *FString::FromInt( SQLITE_OK );

	// ------------------------------------------------------------------------
	// - 
	// ------------------------------------------------------------------------

	const auto RouteToDone = CompilerContext.SpawnIntermediateNode<UK2Node_Knot>( this, SourceGraph );
	RouteToDone->AllocateDefaultPins();

	const auto RouteToDone_Input = RouteToDone->GetInputPin();
	const auto RouteToDone_Output = RouteToDone->GetOutputPin();

	CompilerContext.MovePinLinksToIntermediate( *DonePin, *RouteToDone_Output );

	// ------------------------------------------------------------------------
	// - 
	// ------------------------------------------------------------------------

	const auto RouteToError = CompilerContext.SpawnIntermediateNode<UK2Node_Knot>( this, SourceGraph );
	RouteToError->AllocateDefaultPins();

	const auto RouteToError_Input = RouteToError->GetInputPin();
	const auto RouteToError_Output = RouteToError->GetOutputPin();

	CompilerContext.MovePinLinksToIntermediate( *ErrorPin, *RouteToError_Output );

	// ------------------------------------------------------------------------
	// - Create Assignment node (Exec) to set value to false ------------------
	// ------------------------------------------------------------------------

	const auto AssignBreakVariableFromExec = CompilerContext.SpawnIntermediateNode< UK2Node_AssignmentStatement >( this, SourceGraph );
	AssignBreakVariableFromExec->AllocateDefaultPins();

	const auto AssignBreakVariableFromExec_Exec = AssignBreakVariableFromExec->GetExecPin();
	const auto AssignBreakVariableFromExec_Variable = AssignBreakVariableFromExec->GetVariablePin();
	const auto AssignBreakVariableFromExec_Value = AssignBreakVariableFromExec->GetValuePin();
	const auto AssignBreakVariableFromExec_Then = AssignBreakVariableFromExec->GetThenPin();

	CompilerContext.MovePinLinksToIntermediate( *ExecPin, *AssignBreakVariableFromExec_Exec );
	K2Schema->TryCreateConnection( AssignBreakVariableFromExec_Variable, BreakVariable_Variable );
	AssignBreakVariableFromExec_Value->DefaultValue = TEXT( "0" );

	// ------------------------------------------------------------------------
	// - Create Assignment node (Break) to set value to true ------------------
	// ------------------------------------------------------------------------

	const auto AssignBreakVariableFromBreak = CompilerContext.SpawnIntermediateNode< UK2Node_AssignmentStatement >( this, SourceGraph );
	AssignBreakVariableFromBreak->AllocateDefaultPins();

	const auto AssignBreakVariableFromBreak_Exec = AssignBreakVariableFromBreak->GetExecPin();
	const auto AssignBreakVariableFromBreak_Variable = AssignBreakVariableFromBreak->GetVariablePin();
	const auto AssignBreakVariableFromBreak_Value = AssignBreakVariableFromBreak->GetValuePin();
	const auto AssignBreakVariableFromBreak_Then = AssignBreakVariableFromBreak->GetThenPin();

	CompilerContext.MovePinLinksToIntermediate( *BreakPin, *AssignBreakVariableFromBreak_Exec );
	K2Schema->TryCreateConnection( AssignBreakVariableFromBreak_Variable, BreakVariable_Variable );
	AssignBreakVariableFromBreak_Value->DefaultValue = TEXT( "1" );

	// ------------------------------------------------------------------------
	// - Create Assignment node (from Exec) to cache statement ----------------
	// ------------------------------------------------------------------------

	const auto AssignStatementVariableFromExec = CompilerContext.SpawnIntermediateNode< UK2Node_AssignmentStatement >( this, SourceGraph );
	AssignStatementVariableFromExec->AllocateDefaultPins();

	const auto AssignStatementVariableFromExec_Exec = AssignStatementVariableFromExec->GetExecPin();
	const auto AssignStatementVariableFromExec_Variable = AssignStatementVariableFromExec->GetVariablePin();
	const auto AssignStatementVariableFromExec_Value = AssignStatementVariableFromExec->GetValuePin();
	const auto AssignStatementVariableFromExec_Then = AssignStatementVariableFromExec->GetThenPin();

	K2Schema->TryCreateConnection( AssignBreakVariableFromExec_Then, AssignStatementVariableFromExec_Exec );
	K2Schema->TryCreateConnection( AssignStatementVariableFromExec_Variable, StatementVariable_Variable );
	CompilerContext.MovePinLinksToIntermediate( *StatementInPin, *AssignStatementVariableFromExec_Value );

	// ------------------------------------------------------------------------
	// - Call USqliteStatement::Step ------------------------------------------
	// ------------------------------------------------------------------------

	const auto StepFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	StepFunction->FunctionReference.SetExternalMember( GET_FUNCTION_NAME_CHECKED( USqliteStatement, Step ), USqliteStatement::StaticClass() );
	StepFunction->AllocateDefaultPins();

	const auto StepFunction_Exec = StepFunction->GetExecPin();
	const auto StepFunction_Target = StepFunction->FindPinChecked( TEXT( "Self" ) );
	const auto StepFunction_ReturnValue = StepFunction->GetReturnValuePin();
	const auto StepFunction_Then = StepFunction->GetThenPin();

	K2Schema->TryCreateConnection( AssignStatementVariableFromExec_Then, StepFunction_Exec );
	K2Schema->TryCreateConnection( StatementVariable_Variable, StepFunction_Target );

	// ------------------------------------------------------------------------
	// - ReturnCode variable --------------------------------------------------
	// ------------------------------------------------------------------------

	// Create variable

	// Assign variable (from StepFunction call)

	const auto AssignReturnCodeVariableFromStep = CompilerContext.SpawnIntermediateNode< UK2Node_AssignmentStatement >( this, SourceGraph );
	AssignReturnCodeVariableFromStep->AllocateDefaultPins();

	const auto AssignReturnCodeVariableFromStep_Exec = AssignReturnCodeVariableFromStep->GetExecPin();
	const auto AssignReturnCodeVariableFromStep_Variable = AssignReturnCodeVariableFromStep->GetVariablePin();
	const auto AssignReturnCodeVariableFromStep_Value = AssignReturnCodeVariableFromStep->GetValuePin();
	const auto AssignReturnCodeVariableFromStep_Then = AssignReturnCodeVariableFromStep->GetThenPin();

	K2Schema->TryCreateConnection( StepFunction_Then, AssignReturnCodeVariableFromStep_Exec );
	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromStep_Variable, ReturnCodeVariable_Variable );
	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromStep_Value, StepFunction_ReturnValue );

	// ------------------------------------------------------------------------
	// - Check if ReturnCode is ROW ------------------------------------------- 
	// ------------------------------------------------------------------------

	// Enum comparison "row"

	const auto CompareReturnCodeRow = CompilerContext.SpawnIntermediateNode< UK2Node_CallFunction >( this, SourceGraph );
	CompareReturnCodeRow->FunctionReference.SetExternalMember( GET_FUNCTION_NAME_CHECKED( UKismetMathLibrary, EqualEqual_IntInt ), UKismetMathLibrary::StaticClass() );
	CompareReturnCodeRow->AllocateDefaultPins();

	const auto CompareReturnCodeRow_Input1 = CompareReturnCodeRow->FindPinChecked( TEXT( "A" ) );
	const auto CompareReturnCodeRow_Input2 = CompareReturnCodeRow->FindPinChecked( TEXT( "B" ) );
	const auto CompareReturnCodeRow_ReturnValue = CompareReturnCodeRow->GetReturnValuePin();

	K2Schema->TryCreateConnection( ReturnCodeVariable_Variable, CompareReturnCodeRow_Input1 );
	CompareReturnCodeRow_Input2->DefaultValue = FString::FromInt( SQLITE_ROW );

	// Branch

	const auto BranchReturnCodeRow = CompilerContext.SpawnIntermediateNode< UK2Node_IfThenElse >( this, SourceGraph );
	BranchReturnCodeRow->AllocateDefaultPins();

	const auto BranchReturnCodeRow_Exec = BranchReturnCodeRow->GetExecPin();
	const auto BranchReturnCodeRow_Condition = BranchReturnCodeRow->GetConditionPin();
	const auto BranchReturnCodeRow_Then = BranchReturnCodeRow->GetThenPin();
	const auto BranchReturnCodeRow_Else = BranchReturnCodeRow->GetElsePin();

	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromStep_Then, BranchReturnCodeRow_Exec );
	K2Schema->TryCreateConnection( CompareReturnCodeRow_ReturnValue, BranchReturnCodeRow_Condition );

	// ------------------------------------------------------------------------
	// - Check if ReturnCode is DONE ------------------------------------------ 
	// ------------------------------------------------------------------------

	// Enum comparison "Done"

	const auto CompareReturnCodeDone = CompilerContext.SpawnIntermediateNode< UK2Node_CallFunction >( this, SourceGraph );
	CompareReturnCodeDone->FunctionReference.SetExternalMember( GET_FUNCTION_NAME_CHECKED( UKismetMathLibrary, EqualEqual_IntInt ), UKismetMathLibrary::StaticClass() );
	CompareReturnCodeDone->AllocateDefaultPins();

	const auto CompareReturnCodeDone_Input1 = CompareReturnCodeDone->FindPinChecked( TEXT( "A" ) );
	const auto CompareReturnCodeDone_Input2 = CompareReturnCodeDone->FindPinChecked( TEXT( "B" ) );
	const auto CompareReturnCodeDone_ReturnValue = CompareReturnCodeDone->GetReturnValuePin();

	K2Schema->TryCreateConnection( ReturnCodeVariable_Variable, CompareReturnCodeDone_Input1 );
	CompareReturnCodeDone_Input2->DefaultValue = FString::FromInt( SQLITE_DONE );

	// Branch

	const auto BranchReturnCodeDone = CompilerContext.SpawnIntermediateNode< UK2Node_IfThenElse >( this, SourceGraph );
	BranchReturnCodeDone->AllocateDefaultPins();

	const auto BranchReturnCodeDone_Exec = BranchReturnCodeDone->GetExecPin();
	const auto BranchReturnCodeDone_Condition = BranchReturnCodeDone->GetConditionPin();
	const auto BranchReturnCodeDone_Then = BranchReturnCodeDone->GetThenPin();
	const auto BranchReturnCodeDone_Else = BranchReturnCodeDone->GetElsePin();

	K2Schema->TryCreateConnection( BranchReturnCodeRow_Else, BranchReturnCodeDone_Exec );
	K2Schema->TryCreateConnection( CompareReturnCodeDone_ReturnValue, BranchReturnCodeDone_Condition );

	K2Schema->TryCreateConnection( RouteToError_Input, BranchReturnCodeDone_Else );

	// ------------------------------------------------------------------------
	// - 
	// ------------------------------------------------------------------------

	const auto LoopSequence = CompilerContext.SpawnIntermediateNode< UK2Node_ExecutionSequence >( this, SourceGraph );
	LoopSequence->AllocateDefaultPins();

	const auto Sequence_Exec = LoopSequence->GetExecPin();
	const auto Sequence_One = LoopSequence->GetThenPinGivenIndex( 0 );
	const auto Sequence_Two = LoopSequence->GetThenPinGivenIndex( 1 );

	K2Schema->TryCreateConnection( BranchReturnCodeRow_Then, Sequence_Exec );

	// Sequence One (loop body)

	CompilerContext.MovePinLinksToIntermediate( *ForEachPin, *Sequence_One );

	// Sequence Two (check for break)

	const auto BranchBreak = CompilerContext.SpawnIntermediateNode< UK2Node_IfThenElse >( this, SourceGraph );
	BranchBreak->AllocateDefaultPins();

	const auto BranchBreak_Exec = BranchBreak->GetExecPin();
	const auto BranchBreak_Condition = BranchBreak->GetConditionPin();
	const auto BranchBreak_Then = BranchBreak->GetThenPin();
	const auto BranchBreak_Else = BranchBreak->GetElsePin();

	K2Schema->TryCreateConnection( BranchBreak_Exec, Sequence_Two );
	K2Schema->TryCreateConnection( BreakVariable_Variable, BranchBreak_Condition );

	K2Schema->TryCreateConnection( BranchBreak_Else, StepFunction_Exec );

	// ------------------------------------------------------------------------
	// - Finalize -------------------------------------------------------------
	// ------------------------------------------------------------------------

	const auto BranchFinalize = CompilerContext.SpawnIntermediateNode< UK2Node_IfThenElse >( this, SourceGraph );
	BranchFinalize->AllocateDefaultPins();

	const auto BranchFinalize_Exec = BranchFinalize->GetExecPin();
	const auto BranchFinalize_Condition = BranchFinalize->GetConditionPin();
	const auto BranchFinalize_Then = BranchFinalize->GetThenPin();
	const auto BranchFinalize_Else = BranchFinalize->GetElsePin();

	K2Schema->TryCreateConnection( BranchReturnCodeDone_Then, BranchFinalize_Exec );
	K2Schema->TryCreateConnection( BranchBreak_Then, BranchFinalize_Exec );
	CompilerContext.MovePinLinksToIntermediate( *FinalizePin, *BranchFinalize_Condition );
	K2Schema->TryCreateConnection( RouteToDone_Input, BranchFinalize_Else );

	// Call USqliteStatement::Finalize

	const auto FinalizeFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>( this, SourceGraph );
	FinalizeFunction->FunctionReference.SetExternalMember( GET_FUNCTION_NAME_CHECKED( USqliteStatement, Finalize ), USqliteStatement::StaticClass() );
	FinalizeFunction->AllocateDefaultPins();

	const auto FinalizeFunction_Exec = FinalizeFunction->GetExecPin();
	const auto FinalizeFunction_Target = FinalizeFunction->FindPinChecked( TEXT( "Self" ) );
	const auto FinalizeFunction_ReturnValue = FinalizeFunction->GetReturnValuePin();
	const auto FinalizeFunction_Then = FinalizeFunction->GetThenPin();

	K2Schema->TryCreateConnection( BranchFinalize_Then, FinalizeFunction_Exec );
	K2Schema->TryCreateConnection( StatementVariable_Variable, FinalizeFunction_Target );

	// Assign variable (from FinalizeFunction call)

	const auto AssignReturnCodeVariableFromFinalize = CompilerContext.SpawnIntermediateNode< UK2Node_AssignmentStatement >( this, SourceGraph );
	AssignReturnCodeVariableFromFinalize->AllocateDefaultPins();

	const auto AssignReturnCodeVariableFromFinalize_Exec = AssignReturnCodeVariableFromFinalize->GetExecPin();
	const auto AssignReturnCodeVariableFromFinalize_Variable = AssignReturnCodeVariableFromFinalize->GetVariablePin();
	const auto AssignReturnCodeVariableFromFinalize_Value = AssignReturnCodeVariableFromFinalize->GetValuePin();
	const auto AssignReturnCodeVariableFromFinalize_Then = AssignReturnCodeVariableFromFinalize->GetThenPin();

	K2Schema->TryCreateConnection( FinalizeFunction_Then, AssignReturnCodeVariableFromFinalize_Exec );
	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromFinalize_Variable, ReturnCodeVariable_Variable );
	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromFinalize_Value, FinalizeFunction_ReturnValue );

	// Check if ReturnCode is OK

	const auto CompareReturnCodeOk = CompilerContext.SpawnIntermediateNode< UK2Node_CallFunction >( this, SourceGraph );
	CompareReturnCodeOk->FunctionReference.SetExternalMember( GET_FUNCTION_NAME_CHECKED( UKismetMathLibrary, EqualEqual_IntInt ), UKismetMathLibrary::StaticClass() );
	CompareReturnCodeOk->AllocateDefaultPins();

	const auto CompareReturnCodeOk_Input1 = CompareReturnCodeOk->FindPinChecked( TEXT( "A" ) );
	const auto CompareReturnCodeOk_Input2 = CompareReturnCodeOk->FindPinChecked( TEXT( "B" ) );
	const auto CompareReturnCodeOk_ReturnValue = CompareReturnCodeOk->GetReturnValuePin();

	K2Schema->TryCreateConnection( ReturnCodeVariable_Variable, CompareReturnCodeOk_Input1 );
	CompareReturnCodeOk_Input2->DefaultValue = FString::FromInt( SQLITE_OK );

	// Branch

	const auto BranchReturnCodeOk = CompilerContext.SpawnIntermediateNode< UK2Node_IfThenElse >( this, SourceGraph );
	BranchReturnCodeOk->AllocateDefaultPins();

	const auto BranchReturnCodeOk_Exec = BranchReturnCodeOk->GetExecPin();
	const auto BranchReturnCodeOk_Condition = BranchReturnCodeOk->GetConditionPin();
	const auto BranchReturnCodeOk_Then = BranchReturnCodeOk->GetThenPin();
	const auto BranchReturnCodeOk_Else = BranchReturnCodeOk->GetElsePin();

	K2Schema->TryCreateConnection( AssignReturnCodeVariableFromFinalize_Then, BranchReturnCodeOk_Exec );
	K2Schema->TryCreateConnection( CompareReturnCodeOk_ReturnValue, BranchReturnCodeOk_Condition );

	K2Schema->TryCreateConnection( RouteToDone_Input, BranchReturnCodeOk_Then );
	K2Schema->TryCreateConnection( RouteToError_Input, BranchReturnCodeOk_Else );

	// ------------------------------------------------------------------------
	// - Link output variables ------------------------------------------------
	// ------------------------------------------------------------------------

	CompilerContext.MovePinLinksToIntermediate( *StatementOutPin, *StatementVariable_Variable );
	StatementOutPin->PinType = StatementVariable_Variable->PinType;

	CompilerContext.MovePinLinksToIntermediate( *ReturnCodePin, *ReturnCodeVariable_Variable );
	ReturnCodePin->PinType = ReturnCodeVariable_Variable->PinType;

	// ---------------------------------------------------------------------------
	// - Break all node links ----------------------------------------------------
	// ---------------------------------------------------------------------------

	BreakAllNodeLinks();
}

// ============================================================================
// = 
// ============================================================================

bool UK2Node_Step::CheckForErrors( const FKismetCompilerContext& CompilerContext )
{
	bool bError = false;

	if( GetStatementInPin()->LinkedTo.Num() == 0 )
	{
		CompilerContext.MessageLog.Error( *LOCTEXT( "MissingStatementIn_Error", "Node '@@' must have a statement to process." ).ToString(), this );
		bError = true;
	}

	return bError;
}

// ============================================================================
// = 
// ============================================================================

UEdGraphPin* UK2Node_Step::GetStatementInPin( void ) const
{
	return FindPinChecked( PN_StatementIn );
}

UEdGraphPin* UK2Node_Step::GetFinalizePin( void ) const
{
	return FindPinChecked( PN_Finalize );
}

UEdGraphPin* UK2Node_Step::GetBreakPin( void ) const
{
	return FindPinChecked( PN_Break );
}

// ----------------------------------------------------------------------------

UEdGraphPin* UK2Node_Step::GetForEachPin( void ) const
{
	return FindPinChecked( PN_ForEachRow );
}

UEdGraphPin* UK2Node_Step::GetStatementOutPin( void ) const
{
	return FindPinChecked( PN_StatementOut );
}

UEdGraphPin* UK2Node_Step::GetDonePin( void ) const
{
	return FindPinChecked( PN_Done );
}

UEdGraphPin* UK2Node_Step::GetErrorPin( void ) const
{
	return FindPinChecked( PN_Error );
}

UEdGraphPin* UK2Node_Step::GetReturnCodePin( void ) const
{
	return FindPinChecked( PN_ReturnCode );
}

// ============================================================================
// = 
// ============================================================================

FText UK2Node_Step::GetNodeTitle( ENodeTitleType::Type TitleType ) const
{
	return LOCTEXT( "NodeTitle_NONE", "Step (Evaluate statement)" );
}

FText UK2Node_Step::GetTooltipText() const
{
	return LOCTEXT( "NodeToolTip", "Evaluate a prepared statement and loop over each row of result" );
}

FText UK2Node_Step::GetMenuCategory() const
{
	return LOCTEXT( "NodeMenu", "Sqlite3|Statement" );
}

FSlateIcon UK2Node_Step::GetIconAndTint( FLinearColor& OutColor ) const
{
	return FSlateIcon( "EditorStyle", "GraphEditor.Macro.ForEach_16x" );
}

void UK2Node_Step::GetMenuActions( FBlueprintActionDatabaseRegistrar& ActionRegistrar ) const
{
	K2Utils::DefaultGetMenuActions( this, ActionRegistrar );
}

#undef LOCTEXT_NAMESPACE
