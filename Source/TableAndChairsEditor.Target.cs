using UnrealBuildTool;
using System.Collections.Generic;

public class TableAndChairsEditorTarget : TargetRules
{
	public TableAndChairsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "TableAndChairs" } );
	}
}
